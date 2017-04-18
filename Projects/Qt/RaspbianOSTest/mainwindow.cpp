#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->bleSemaphore = new QSemaphore(0);
    this->stringQueue = new QQueue<QByteArray>();

    this->bleSerial = new QSerialPort("/dev/ttyS0");
    this->bleSerial->setBaudRate(BLEBAUDRATE);
    this->bleSerial->setDataBits(QSerialPort::Data8);
    this->bleSerial->setParity(QSerialPort::NoParity);
    this->bleSerial->setStopBits(QSerialPort::OneStop);
    this->bleSerial->setFlowControl(QSerialPort::NoFlowControl);
    this->bleSerial->open(QIODevice::ReadWrite);

    this->logfile = new QFile(LOGFILENAME);
    this->logfile->open(QIODevice::WriteOnly | QIODevice::Append);

    connect(this, SIGNAL(errorMessage(QString,QString)), this, SLOT(errorMessageReceived(QString,QString)));

    this->initGPIO();
    this->initI2C();
    this->initBLE();

    this->logfileThreadAbort = false;
    this->adcThreadAbort = false;
    this->bleThreadAbort = false;

    this->logfileThread = QtConcurrent::run(this, &MainWindow::dumpToFile);
    this->adcThread = QtConcurrent::run(this, &MainWindow::readADCPeriodically);
    this->bleThread = QtConcurrent::run(this, &MainWindow::readBLEPeriodically);
}

MainWindow::~MainWindow()
{
    this->bleSerial->close();
    delete this->bleSerial;

    this->logfile->close();
    delete this->logfile;

    this->logfileThreadAbort = true;
    this->adcThreadAbort = true;
    this->bleThreadAbort = true;

    this->logfileThread.waitForFinished();
    this->adcThread.waitForFinished();
    this->bleThread.waitForFinished();

    delete ui;
}

void MainWindow::initGPIO()
{
    // Initialize GPIO pins
    // LED0
    this->LED0Pin.Init(GPIOLED0, GPIO_Out);
    this->LED0Pin.Write(GPIO_Low);

    // LED1
    this->LED1Pin.Init(GPIOLED1, GPIO_Out);
    this->LED1Pin.Write(GPIO_Low);

    this->SW0Pin.Init(GPIOSW0, GPIO_In);
    // Jelenlegi állapot jelzése
    if(this->SW0Pin.Read() == GPIO_Low)
        ui->checkBoxSw0->setChecked(false);
    else
        ui->checkBoxSw0->setChecked(true);
    // Edge signal engedélyezése
    this->SW0Pin.WatchEdge(GPIO_EdgeDetectEnabled);

    this->SW1Pin.Init(GPIOSW1, GPIO_In);
    // Jelenlegi állapot jelzése
    if(this->SW1Pin.Read() == GPIO_Low)
        ui->checkBoxSw1->setChecked(false);
    else
        ui->checkBoxSw1->setChecked(true);
    // Edge signal engedélyezése
    this->SW1Pin.WatchEdge(GPIO_EdgeDetectEnabled);

    // In
    this->INPin.Init(GPIOIN, GPIO_In);
    this->INPin.WatchEdge(GPIO_EdgeDetectEnabled);

    // Out
    this->OUTPin.Init(GPIOOUT, GPIO_Out);
    this->OUTPin.Write(this->INPin.Read());

    // BLERst
    this->BLERstPin.Init(GPIOBLERST, GPIO_Out);
    this->BLERstPin.Write(GPIO_Low);

    // ADCRst
    this->ADCRstPin.Init(GPIOADCRST, GPIO_Out);
    this->ADCRstPin.Write(GPIO_Low);

    // Aszinkron jelzések összekapcsolása a hozzájuk tartozó függvénnyel
    connect(&this->SW0Pin, SIGNAL(EdgeDetected(int)), this, SLOT(SW0ValueChange(int)));
    connect(&this->SW1Pin, SIGNAL(EdgeDetected(int)), this, SLOT(SW1ValueChange(int)));
    connect(&this->INPin, SIGNAL(EdgeDetected(int)), this, SLOT(InValueChange(int)));
}

void MainWindow::initI2C()
{
    this->i2cDevice = wiringPiI2CSetup(ADC_I2C_ADDR);

    this->ADCRstPin.Write(GPIO_High);
    QThread::msleep(1);

    wiringPiI2CWriteReg8(this->i2cDevice,ADC_I2C_MODE_REG, ADC_I2C_MODE_AWAKE_CH0);
    wiringPiI2CWriteReg8(this->i2cDevice, ADC_I2C_INT_CNTRL_REG, ADC_I2C_DISABLE_INT);

    connect(this, SIGNAL(newLocalTempData(int)), this, SLOT(newLocalTempDataSlot(int)));
    connect(this, SIGNAL(newPotmeterData(int)), this, SLOT(newPotmeterDataSlot(int)));
}

void MainWindow::initBLE()
{
    connect(this->bleSerial, SIGNAL(readyRead()), this, SLOT(BLESerialDataReceived()));
    connect(&this->bglib, SIGNAL(SystemBootEvent(QObject*,BGLib::BootEventArgs)), this, SLOT(SystemBootEvent(QObject*,BGLib::BootEventArgs)));
    connect(&this->bglib, SIGNAL(SystemHelloResponse(QObject*,BGLib::HelloEventArgs)), this, SLOT(HelloResponse(QObject*,BGLib::HelloEventArgs)));
    connect(&this->bglib, SIGNAL(GAPConnectDirectResponse(QObject*,BGLib::ConnectDirectEventArgs)), this, SLOT(GAPConnectDirectResponse(QObject*,BGLib::ConnectDirectEventArgs)));
    connect(&this->bglib, SIGNAL(ConnectionStatusEvent(QObject*,BGLib::ConnectionStatusEventArgs)), this, SLOT(ConnectionStatusEvent(QObject*,BGLib::ConnectionStatusEventArgs)));
    connect(&this->bglib, SIGNAL(ATTClientProcedureCompletedEvent(QObject*,BGLib::ProcedureCompletedEventArgs)), this, SLOT(ATTClientProcedureCompletedEvent(QObject*,BGLib::ProcedureCompletedEventArgs)));
    connect(&this->bglib, SIGNAL(ATTClientAttributeValueEvent(QObject*,BGLib::AttributeValueEventArgs)), this, SLOT(ATTClientAttributeValueEvent(QObject*,BGLib::AttributeValueEventArgs)));

    connect(this, SIGNAL(newAmbTempData(int)), this, SLOT(newAmbTempDataSlot(int)));
    connect(this, SIGNAL(newRemTempData(int)), this, SLOT(newRemTempDataSlot(int)));
    connect(this, SIGNAL(newLightData(float)), this, SLOT(newLightDataSlot(float)));

    connect(this, SIGNAL(sendBLECommand(QByteArray)), this, SLOT(sendBLECommandSlot(QByteArray)));

    this->bglib.SetPacketMode(true);
    this->BLERstPin.Write(GPIO_High);
}

void MainWindow::dumpToFile()
{
    forever
    {
        if(this->logfileThreadAbort)
            return;

        while(!this->stringQueue->isEmpty())
        {
            this->logfile->write(this->stringQueue->dequeue());
            this->logfile->flush();
        }
        QThread::msleep(1000);
    }
}

void MainWindow::readADCPeriodically()
{
    int result;

    forever
    {
        if(this->adcThreadAbort)
            return;

        wiringPiI2CWriteReg8(this->i2cDevice, ADC_I2C_MODE_REG, ADC_I2C_DATA0_CONV_START);
        QThread::msleep(15);
        wiringPiI2CWriteReg8(this->i2cDevice, ADC_I2C_MODE_REG, ADC_I2C_DATA0_CONV_START);
        QThread::msleep(1);
        result = wiringPiI2CReadReg16(this->i2cDevice, ADC_I2C_DATA0_REG | ADC_I2C_READ_INC);
        // Byte-swap
        result = (((result >> 8) & 0xFF) | ((result << 8) & 0xFF00));
        this->processLocalTemp(result);

        wiringPiI2CWriteReg8(this->i2cDevice, ADC_I2C_MODE_REG, ADC_I2C_DATA1_CONV_START);
        QThread::msleep(15);
        wiringPiI2CWriteReg8(this->i2cDevice, ADC_I2C_MODE_REG, ADC_I2C_DATA1_CONV_START);
        QThread::msleep(1);
        result = wiringPiI2CReadReg16(this->i2cDevice, ADC_I2C_DATA1_REG | ADC_I2C_READ_INC);
        // Byte-swap
        result = (((result >> 8) & 0xFF) | ((result << 8) & 0xFF00));
        this->processPotmeter(result);

        QThread::msleep(500);
    }
}

void MainWindow::readBLEPeriodically()
{
    bool rsp;

    forever
    {
        QThread::msleep(1000);
        this->bleSemaphore->tryAcquire(1);
        do
        {
            if(this->bleThreadAbort)
                return;

            emit this->sendBLECommand(bglib.BLECommandSystemReset(0));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        } while(!rsp);

        if(this->bleThreadAbort)
            return;

        if(rsp)
        {
            emit this->sendBLECommand(bglib.BLECommandSystemHello());
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        if(rsp)
        {
            char address[] = {SENSORTAG_ADDRESS_0, SENSORTAG_ADDRESS_1, SENSORTAG_ADDRESS_2, SENSORTAG_ADDRESS_3, SENSORTAG_ADDRESS_4, SENSORTAG_ADDRESS_5};
            emit this->sendBLECommand(bglib.BLECommandGAPConnectDirect(address, 0, BLECONNINTERVALMIN, BLECONNINTERVALMAX, BLECONNTIMEOUT, BLECONNSLAVELATENCY));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        // Config temperature sensor period
        if(rsp)
        {
            char data[] = {100};
            emit this->sendBLECommand(bglib.BLECommandATTClientAttributeWrite(this->bleConnectionHndl, BLETEMPPERIODHNDL, QByteArray(data, sizeof(data))));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        // Config light sensor period
        if(rsp)
        {
            char data[] = {100};
            bglib.SendCommand(this->bleSerial, bglib.BLECommandATTClientAttributeWrite(this->bleConnectionHndl, BLELIGHTPERIODHNDL, QByteArray(data, sizeof(data))));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        // Start temperature measurement
        if(rsp)
        {
            char data[] = {0x01};
            emit this->sendBLECommand(bglib.BLECommandATTClientAttributeWrite(this->bleConnectionHndl, BLETEMPCONFIGHNDL, QByteArray(data, sizeof(data))));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        // Start light measurement
        if(rsp)
        {
            char data[] = {0x01};
            emit this->sendBLECommand(bglib.BLECommandATTClientAttributeWrite(this->bleConnectionHndl, BLELIGHTCONFIGHNDL, QByteArray(data, sizeof(data))));
            rsp = this->bleSemaphore->tryAcquire(1,2000);
        }
        else
        {
            emit this->errorMessage("", "Semaphore timeout");
        }

        if(this->bleThreadAbort)
            return;

        while(rsp)
        {
            QThread::msleep(1000);

            if(this->bleThreadAbort)
                return;

            // Read temperature data
            if(rsp)
            {
                emit this->sendBLECommand(bglib.BLECommandATTClientReadByHandle(this->bleConnectionHndl, BLETEMPDATAHNDL));
                rsp = this->bleSemaphore->tryAcquire(1,2000);
            }
            else
            {
                emit this->errorMessage("", "Semaphore timeout");
            }

            if(this->bleThreadAbort)
                return;

            // Read light data
            if(rsp)
            {
                emit this->sendBLECommand(bglib.BLECommandATTClientReadByHandle(this->bleConnectionHndl, BLELIGHTDATAHNDL));
                rsp = this->bleSemaphore->tryAcquire(1,2000);
            }
            else
            {
                emit this->errorMessage("", "Semaphore timeout");
            }

            if(this->bleThreadAbort)
                return;
        }

        emit this->sendBLECommand(bglib.BLECommandConnectionDisconnect(this->bleConnectionHndl));
    }
}

void MainWindow::processLocalTemp(int data)
{
    int val = data >> 4;
    int celsValue = 300*val/4095 - 50;

    emit this->newLocalTempData(celsValue);
    this->stringQueue->enqueue("Local temp: " + QByteArray::number(celsValue) + "°C\n");
}

void MainWindow::processPotmeter(int data)
{
    int val = data >> 4;
    int potValue = 100*val/4095;

    emit this->newPotmeterData(potValue);
    this->stringQueue->enqueue("Potmeter: " + QByteArray::number(potValue) + "%\n");
}

void MainWindow::SW0ValueChange(int GPIOState)
{
    // Új érték jelzése
    if(GPIOState == GPIO_Low)
    {
        ui->checkBoxSw0->setChecked(false);
        this->stringQueue->enqueue("SW0 LOW\n");
    }
    else
    {
        ui->checkBoxSw0->setChecked(true);
        this->stringQueue->enqueue("SW0 HIGH\n");
    }
}

void MainWindow::SW1ValueChange(int GPIOState)
{
    // Új érték jelzése
    if(GPIOState == GPIO_Low)
    {
        ui->checkBoxSw1->setChecked(false);
        this->stringQueue->enqueue("SW1 LOW\n");
    }
    else
    {
        ui->checkBoxSw1->setChecked(true);
        this->stringQueue->enqueue("SW1 HIGH\n");
    }
}

void MainWindow::InValueChange(int GPIOState)
{
    // In értékének kitétele az Out lábra
    this->OUTPin.Write(GPIOState);
}

// BGLib Responses
void MainWindow::HelloResponse(QObject *sender, BGLib::HelloEventArgs e)
{
    Q_UNUSED(sender)
    Q_UNUSED(e)

    qDebug() << "Hello response";
    this->bleSemaphore->release(1);
}

void MainWindow::GAPConnectDirectResponse(QObject *sender, BGLib::ConnectDirectEventArgs e)
{
    Q_UNUSED(sender)

    qDebug() << "ConnectDirect response";
    this->bleConnectionHndl = e.connection_handle;
}

// BGLib Events
void MainWindow::SystemBootEvent(QObject *sender, BGLib::BootEventArgs e)
{
    Q_UNUSED(sender)
    Q_UNUSED(e)

    qDebug() << "SystemBoot event";
    this->bleSemaphore->release(1);
}

void MainWindow::ConnectionStatusEvent(QObject *sender, BGLib::ConnectionStatusEventArgs e)
{
    Q_UNUSED(sender)

    qDebug() << "ConnectionStatus event";
    if((e.flags & 0x01) != 0) // connection_connected
    {
        emit this->errorMessage("", "");
        this->bleSemaphore->release(1);
    }
}

void MainWindow::ATTClientProcedureCompletedEvent(QObject *sender, BGLib::ProcedureCompletedEventArgs e)
{
    Q_UNUSED(sender)

    qDebug() << "ProcedureCompleted event";
    // Ha Write parancsra érkezett, és sikeres a folyamat...
    if(e.connection == this->bleConnectionHndl)
        if(e.chrhandle == BLETEMPPERIODHNDL || e.chrhandle == BLETEMPCONFIGHNDL || e.chrhandle == BLELIGHTPERIODHNDL || e.chrhandle == BLELIGHTCONFIGHNDL)
            if(e.result == 0)
            {
                this->bleSemaphore->release(1);
            }
}

void MainWindow::ATTClientAttributeValueEvent(QObject *sender, BGLib::AttributeValueEventArgs e)
{
    Q_UNUSED(sender)

    qDebug() << "AttributeValue event";
    // Adatolvasás sikeresen megtörtént
    if(e.connection == this->bleConnectionHndl)
    {
        if(e.atthandle == BLETEMPDATAHNDL)
        {
            if(e.value.length() == 4)
            {
                short ambTemp = (short)(e.value.at(1) << 8 | e.value.at(0));
                short remTemp = (short)(e.value.at(3) << 8 | e.value.at(2));

                float ambTemp_f = (float)(ambTemp >> 2);
                float remTemp_f = (float)(remTemp >> 2);

                float ambTempVal = ambTemp_f / 32;
                float remTempVal = remTemp_f / 32;

                emit this->newAmbTempData((int)ambTempVal);
                emit this->newRemTempData((int)remTempVal);

                this->stringQueue->enqueue("Ambient temp: " + QByteArray::number((int)ambTempVal) + "°C\n");
                this->stringQueue->enqueue("Remote temp: " + QByteArray::number((int)remTempVal) + "°C\n");
            }
        }
        else if(e.atthandle == BLELIGHTDATAHNDL)
        {
            if(e.value.length() == 2)
            {
                int val = (int)(e.value.at(1) << 8 | e.value.at(0));
                int exp = (val & 0xF000) >> 12;
                int low_limit_res = (val & 0x0FFF);

                float lightVal = (float)(0.01*(1 << exp) * low_limit_res);

                emit this->newLightData(lightVal);

                this->stringQueue->enqueue("Light: " + QByteArray::number(lightVal) + " lux\n");
            }
        }

        this->bleSemaphore->release(1);
    }
}

// BLE serial data received
void MainWindow::BLESerialDataReceived()
{
    //qDebug() << "Serial data received";
    QByteArray receivedData = this->bleSerial->readAll();
    for(int i=0;i<receivedData.length();i++)
        this->bglib.Parse(receivedData.at(i));
}

void MainWindow::on_checkBoxLed0_toggled(bool checked)
{
    // LED0 értékének frissitése
    if(checked)
    {
        this->LED0Pin.Write(GPIO_High);
        this->stringQueue->enqueue("LED0 HIGH\n");
    }
    else
    {
        this->LED0Pin.Write(GPIO_Low);
        this->stringQueue->enqueue("LED0 LOW\n");
    }
}

void MainWindow::on_checkBoxLed1_toggled(bool checked)
{
    // LED1 értékének frissitése
    if(checked)
    {
        this->LED1Pin.Write(GPIO_High);
        this->stringQueue->enqueue("LED1 HIGH\n");
    }
    else
    {
        this->LED1Pin.Write(GPIO_Low);
        this->stringQueue->enqueue("LED1 LOW\n");
    }
}

void MainWindow::newLocalTempDataSlot(int data)
{
    ui->labelLocalTempValue->setText(QString::number(data) + "°C");
}

void MainWindow::newPotmeterDataSlot(int data)
{
    ui->labelPotmeterValue->setText(QString::number(data) + "%");
}

void MainWindow::newAmbTempDataSlot(int data)
{
    ui->labelAmbientTempValue->setText(QString::number(data) + "°C");
}

void MainWindow::newRemTempDataSlot(int data)
{
    ui->labelRemoteTempValue->setText(QString::number(data) + "°C");
}

void MainWindow::newLightDataSlot(float data)
{
    ui->labelLightValue->setText(QString::number(data) + " lux");
}

void MainWindow::errorMessageReceived(QString errorValue, QString errorMessage)
{
    ui->labelErrorValue->setText(errorValue);
    ui->labelErrorMessage->setText(errorMessage);
}

void MainWindow::sendBLECommandSlot(QByteArray command)
{
    qDebug() << "Serial data sent";
    this->bglib.SendCommand(this->bleSerial, command);
}
