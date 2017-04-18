#include "pigpio.h"

PiGPIO::PiGPIO(QObject *parent) :
    QObject(parent)
{
    this->directionFile_p = NULL;
    this->valueFile_p = NULL;
    this->edgeWatcher_p = NULL;
}

PiGPIO::~PiGPIO()
{
    if(this->directionFile_p != NULL)
        delete this->directionFile_p;

    if(this->valueFile_p != NULL)
        delete this->valueFile_p;

    if(this->edgeWatcher_p != NULL)
        delete this->edgeWatcher_p;
}

int PiGPIO::Init(int GPIOPin, int GPIODirection)
{
    this->directoryPath = "/sys/class/gpio/gpio" + QString::number(GPIOPin);

    // Export GPIO pin

    QFile exportFile("/sys/class/gpio/export");
    exportFile.open(QIODevice::WriteOnly);

    exportFile.write(QByteArray::number(GPIOPin));
    exportFile.flush();
    exportFile.close();
    this->edgeWatcher_p = new QFileSystemWatcher();

    QThread::msleep(100);
    while(!QDir(this->directoryPath).exists())
        QThread::msleep(10);

    if(QDir(this->directoryPath).exists())
    {
        this->directionFile_p = new QFile(this->directoryPath + "/direction");
        this->valueFile_p = new QFile(this->directoryPath + "/value");
        this->edgeWatcher_p->addPath(this->directoryPath + "/value");

        if(this->directionFile_p->open(QIODevice::WriteOnly))
        {
            if(GPIODirection == GPIO_In)
                this->directionFile_p->write("in");
            else
                this->directionFile_p->write("out");

            this->directionFile_p->close();
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

int PiGPIO::Read()
{
    if(this->valueFile_p->open(QIODevice::ReadOnly))
    {
        QString value = this->valueFile_p->read(16);
        this->valueFile_p->close();

        if(value.simplified() == "1")
            return GPIO_High;
        else
            return GPIO_Low;
    }
    else
        return -1;
}

int PiGPIO::Write(int GPIOState)
{
    this->valueFile_p->open(QIODevice::WriteOnly);
    if(GPIOState == GPIO_High)
        this->valueFile_p->write("1");
    else
        this->valueFile_p->write("0");

    this->valueFile_p->close();

    return 0;
}

int PiGPIO::WatchEdge(int enable)
{
    if(enable)
    {
        this->previousValue = this->Read();
        QFile edgeFile(this->directoryPath + "/edge");
        edgeFile.open(QIODevice::WriteOnly);
        edgeFile.write("both");
        edgeFile.close();
        connect(this->edgeWatcher_p, SIGNAL(fileChanged(QString)), this, SLOT(TimeOut(QString)));
    }
    else
    {
        disconnect(this->edgeWatcher_p, SIGNAL(fileChanged(QString)), this, SLOT(TimeOut(QString)));
        QFile edgeFile(this->directoryPath + "/edge");
        edgeFile.open(QIODevice::WriteOnly);
        edgeFile.write("none");
        edgeFile.close();
    }

    return 0;
}

void PiGPIO::TimeOut(QString filePath)
{
    Q_UNUSED(filePath);

    int value = this->Read();
    if(this->previousValue != value)
        emit this->EdgeDetected(value);

    this->previousValue = value;
}
