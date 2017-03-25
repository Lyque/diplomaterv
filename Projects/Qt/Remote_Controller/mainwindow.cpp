#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->serial = new QSerialPort(this);
    settings = new SettingsDialog;

    this->ledOn.load(":/images/led_on.png");
    this->ledOff.load(":/images/led_off.png");
    this->switchOn.load(":/images/switch_on.png");
    this->switchOff.load(":/images/switch_off.png");

    this->ledOn.scaledToHeight(40, Qt::TransformationMode::SmoothTransformation);
    this->ledOff.scaledToHeight(40, Qt::TransformationMode::SmoothTransformation);
    this->switchOn.scaledToHeight(20, Qt::TransformationMode::SmoothTransformation);
    this->switchOff.scaledToHeight(20, Qt::TransformationMode::SmoothTransformation);

    ui->led0Indicator->setPixmap(this->ledOff);
    ui->led1Indicator->setPixmap(this->ledOff);
    ui->switch0Indicator->setPixmap(this->switchOff);
    ui->switch1Indicator->setPixmap(this->switchOff);

    ui->tempLocalBar->setValue(2047);
    ui->tempRemoteBar->setValue(32000);
    ui->potmeterDial->setValue(2047);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionSettings->setEnabled(true);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    initActionsConnections();

    connect(this->serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &MainWindow::handleError);
    connect(this->serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(this, SIGNAL(led0ChangedSignal(bool)), this, SLOT(led0ChangedSlot(bool)));
    connect(this, SIGNAL(led1ChangedSignal(bool)), this, SLOT(led1ChangedSlot(bool)));
    connect(this, SIGNAL(switch0ChangedSignal(bool)), this, SLOT(switch0ChangedSlot(bool)));
    connect(this, SIGNAL(switch1ChangedSignal(bool)), this, SLOT(switch1ChangedSlot(bool)));
    connect(this, SIGNAL(localTempChangedSignal(uint32_t)), this, SLOT(localTempChangedSlot(uint32_t)));
    connect(this, SIGNAL(potmeterChangedSignal(uint32_t)), this, SLOT(potmeterChangedSlot(uint32_t)));
}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

void MainWindow::openSerialPort()
{
    QByteArray message = "connect :";
    char value[5];

    value[0] = 0xAA;
    value[1] = 0xAA;
    value[2] = 0xAA;
    value[3] = 0xAA;
    value[4] = '\n';

    message.append(value, 5);

    SettingsDialog::Settings p = settings->settings();
    this->serial->setPortName(p.name);
    this->serial->setBaudRate(p.baudRate);
    this->serial->setDataBits(p.dataBits);
    this->serial->setParity(p.parity);
    this->serial->setStopBits(p.stopBits);
    this->serial->setFlowControl(p.flowControl);
    if (this->serial->open(QIODevice::ReadWrite)) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionSettings->setEnabled(false);
        // Done: Widgetek engedélyezése
        ui->tempLocalBar->setEnabled(true);
        ui->tempLocalVal->setEnabled(true);
        ui->tempRemoteBar->setEnabled(true);
        ui->tempRemoteVal->setEnabled(true);
        ui->potmeterVal->setEnabled(true);
        ui->led0CheckBox->setEnabled(true);
        ui->led1CheckBox->setEnabled(true);
        ui->humVal->setEnabled(true);
        ui->presVal->setEnabled(true);
        ui->lightVal->setEnabled(true);

        writeData(message);

        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), this->serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (this->serial->isOpen())
        this->serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSettings->setEnabled(true);
    // Done: Widgetek letiltása
    ui->tempLocalBar->setEnabled(false);
    ui->tempLocalVal->setEnabled(false);
    ui->tempRemoteBar->setEnabled(false);
    ui->tempRemoteVal->setEnabled(false);
    ui->potmeterVal->setEnabled(false);
    ui->led0CheckBox->setEnabled(false);
    ui->led1CheckBox->setEnabled(false);
    ui->humVal->setEnabled(false);
    ui->presVal->setEnabled(false);
    ui->lightVal->setEnabled(false);

    showStatusMessage(tr("Disconnected"));
}

void MainWindow::writeData(const QByteArray &data)
{
    this->serial->write(data);
}

void MainWindow::readData()
{
    if(this->serial->canReadLine())
    {
        while(this->serial->bytesAvailable() && this->serial->canReadLine())
        {
            QByteArray data = this->serial->readLine(15);

            if(data.length() == 14)
            {
                // ToDo: Felbontani a beérkezett csomagot, majd feldolgozni azt.
                QList<QByteArray> splitted = data.split(':');
                if(splitted.length() == 2)
                {
                    QByteArray entity = splitted.at(0).simplified();
                    QByteArray value = splitted.at(1);

                    if(entity == "led0")
                    {
                        if(uint8_t(value.at(0)) == 0xAA && uint8_t(value.at(1)) == 0xAA && uint8_t(value.at(2)) == 0xAA && uint8_t(value.at(3)) == 0xAA)
                            emit this->led0ChangedSignal(true);
                        else if(uint8_t(value.at(0)) == 0x55 && uint8_t(value.at(1)) == 0x55 && uint8_t(value.at(2)) == 0x55 && uint8_t(value.at(3)) == 0x55)
                            emit this->led0ChangedSignal(false);
                    }
                    else if(entity == "led1")
                    {
                        if(uint8_t(value.at(0)) == 0xAA && uint8_t(value.at(1)) == 0xAA && uint8_t(value.at(2)) == 0xAA && uint8_t(value.at(3)) == 0xAA)
                            emit this->led1ChangedSignal(true);
                        else if(uint8_t(value.at(0)) == 0x55 && uint8_t(value.at(1)) == 0x55 && uint8_t(value.at(2)) == 0x55 && uint8_t(value.at(3)) == 0x55)
                            emit this->led1ChangedSignal(false);
                    }
                    else if(entity == "switch0")
                    {
                        if(uint8_t(value.at(0)) == 0xAA && uint8_t(value.at(1)) == 0xAA && uint8_t(value.at(2)) == 0xAA && uint8_t(value.at(3)) == 0xAA)
                            emit this->switch0ChangedSignal(true);
                        else if(uint8_t(value.at(0)) == 0x55 && uint8_t(value.at(1)) == 0x55 && uint8_t(value.at(2)) == 0x55 && uint8_t(value.at(3)) == 0x55)
                            emit this->switch0ChangedSignal(false);
                    }
                    else if(entity == "switch1")
                    {
                        if(uint8_t(value.at(0)) == 0xAA && uint8_t(value.at(1)) == 0xAA && uint8_t(value.at(2)) == 0xAA && uint8_t(value.at(3)) == 0xAA)
                            emit this->switch1ChangedSignal(true);
                        else if(uint8_t(value.at(0)) == 0x55 && uint8_t(value.at(1)) == 0x55 && uint8_t(value.at(2)) == 0x55 && uint8_t(value.at(3)) == 0x55)
                            emit this->switch1ChangedSignal(false);
                    }
                    else if(entity == "loctemp")
                    {
                        uint32_t tempVal;

                        memcpy(&tempVal,value,4);
                        emit this->localTempChangedSignal(tempVal);
                    }
                    else if(entity == "potmeter")
                    {
                        uint32_t potmeterVal;

                        memcpy(&potmeterVal,value,4);
                        emit this->potmeterChangedSignal(potmeterVal);
                    }
                }
            }
        }
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), this->serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionSettings, &QAction::triggered, settings, &MainWindow::show);
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::on_led0CheckBox_toggled(bool checked)
{
    QByteArray message = "led0    :";
    char value[5];

    if(checked)
    {
        value[0] = 0xAA;
        value[1] = 0xAA;
        value[2] = 0xAA;
        value[3] = 0xAA;
    }
    else
    {
        value[0] = 0x55;
        value[1] = 0x55;
        value[2] = 0x55;
        value[3] = 0x55;
    }

    value[4] = '\n';

    message.append(value, 5);
    writeData(message);
}

void MainWindow::on_led1CheckBox_toggled(bool checked)
{
    QByteArray message = "led1    :";
    char value[5];

    if(checked)
    {
        value[0] = 0xAA;
        value[1] = 0xAA;
        value[2] = 0xAA;
        value[3] = 0xAA;
    }
    else
    {
        value[0] = 0x55;
        value[1] = 0x55;
        value[2] = 0x55;
        value[3] = 0x55;
    }

    value[4] = '\n';

    message.append(value, 5);
    writeData(message);
}

void MainWindow::led0ChangedSlot(bool isOn)
{
    if(isOn)
    {
        ui->led0Indicator->setPixmap(this->ledOn);
    }
    else
    {
        ui->led0Indicator->setPixmap(this->ledOff);
    }
}

void MainWindow::led1ChangedSlot(bool isOn)
{
    if(isOn)
    {
        ui->led1Indicator->setPixmap(this->ledOn);
    }
    else
    {
        ui->led1Indicator->setPixmap(this->ledOff);
    }
}

void MainWindow::switch0ChangedSlot(bool isOn)
{
    if(isOn)
    {
        ui->switch0Indicator->setPixmap(this->switchOn);
    }
    else
    {
        ui->switch0Indicator->setPixmap(this->switchOff);
    }
}

void MainWindow::switch1ChangedSlot(bool isOn)
{
    if(isOn)
    {
        ui->switch1Indicator->setPixmap(this->switchOn);
    }
    else
    {
        ui->switch1Indicator->setPixmap(this->switchOff);
    }
}

void MainWindow::localTempChangedSlot(uint32_t value)
{
    ui->tempLocalBar->setValue(value);
}

void MainWindow::potmeterChangedSlot(uint32_t value)
{
    ui->potmeterDial->setValue(value);
}
