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

    ui->tempLocalBar->setValue(20);
    ui->tempRemoteBar->setValue(20);
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
    connect(this, SIGNAL(remoteTempChangedSignal(uint32_t)), this, SLOT(remoteTempChangedSlot(uint32_t)));
    connect(this, SIGNAL(lightChangedSignal(uint32_t)), this, SLOT(lightChangedSlot(uint32_t)));
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
        ui->ambTempVal->setEnabled(true);
        ui->lightVal->setEnabled(true);

        this->lastCommand = message;
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
    ui->ambTempVal->setEnabled(false);
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
                // Done: Felbontani a beérkezett csomagot, majd feldolgozni azt.
                QList<QByteArray> splitted = data.split(':');
                if(splitted.length() == 2)
                {
                    QByteArray entity = splitted.at(0).simplified();
                    QByteArray value = splitted.at(1);

                    if(entity == "error")
                    {
                        resendLastCommand();
                    }
                    else if(entity == "led0")
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
                    else if(entity == "remtemp")
                    {
                        uint32_t tempVal;

                        memcpy(&tempVal,value,4);
                        emit this->remoteTempChangedSignal(tempVal);
                    }
                    else if(entity == "lux")
                    {
                        uint32_t lightVal;

                        memcpy(&lightVal,value,4);
                        emit this->lightChangedSignal(lightVal);
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

void MainWindow::resendLastCommand()
{
    writeData(this->lastCommand);
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
    this->lastCommand = message;
    writeData(message);
    ui->led0CheckBox->setEnabled(false);
    ui->led1CheckBox->setEnabled(false);
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
    this->lastCommand = message;
    writeData(message);
    ui->led0CheckBox->setEnabled(false);
    ui->led1CheckBox->setEnabled(false);
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
    ui->led0CheckBox->setChecked(isOn);
    ui->led0CheckBox->setEnabled(true);
    ui->led1CheckBox->setEnabled(true);
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
    ui->led1CheckBox->setChecked(isOn);
    ui->led0CheckBox->setEnabled(true);
    ui->led1CheckBox->setEnabled(true);
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
    int celsValue;
    static int updatedLocalTempValue = 20;
    const float filterCoeff = 5;

    celsValue = 300*value/4095-50;
    // IIR filter
    updatedLocalTempValue = ((filterCoeff-1)*updatedLocalTempValue+celsValue)/filterCoeff;

    ui->tempLocalBar->setValue(updatedLocalTempValue);
    ui->tempLocalVal->setText(QString::number(updatedLocalTempValue) + "°C");
    ui->tempLocalBar->update();
}

void MainWindow::potmeterChangedSlot(uint32_t value)
{
    static int updatedPotValue = 2047;
    const float filterCoeff = 3;

    // IIR filter
    updatedPotValue = ((filterCoeff-1)*updatedPotValue+value)/filterCoeff;

    ui->potmeterDial->setValue(updatedPotValue);
    ui->potmeterVal->setText(QString::number(100*updatedPotValue/4095) + "%");
}

void MainWindow::remoteTempChangedSlot(uint32_t value)
{
    static int updatedAmbientTempValue = 50;
    static int updatedSensorTempValue = 20;
    const float filterCoeff = 1.1;

    int16_t ambTemp = (value & 0x0000FFFF);
    int16_t sensTemp = (value & 0xFFFF0000) >> 16;

    float ambTemp_f = float(ambTemp>>2);
    float sensTemp_f = float(sensTemp>>2);

    float ambTempVal = ambTemp_f/32;
    float sensTempVal = sensTemp_f/32;

    // IIR filter
    updatedAmbientTempValue = ((filterCoeff-1)*updatedAmbientTempValue+ambTempVal)/filterCoeff;
    updatedSensorTempValue = ((filterCoeff-1)*updatedSensorTempValue+sensTempVal)/filterCoeff;

    ui->ambTempVal->setText(QString::number(updatedAmbientTempValue) + "°C");
    ui->tempRemoteBar->setValue(updatedSensorTempValue);
    ui->tempRemoteVal->setText(QString::number(updatedSensorTempValue) + "°C");
    ui->tempRemoteBar->update();
}

void MainWindow::lightChangedSlot(uint32_t value)
{
    static float updatedLightValue = 0;
    const float filterCoeff = 1.1;

    int exp = (value & 0xF000) >> 12;
    int low_limit_res = (value & 0x0FFF);
    float lightVal = 0.01*(1 << (exp))*low_limit_res;

    // IIR filter
    updatedLightValue = ((filterCoeff-1)*updatedLightValue+lightVal)/filterCoeff;
    ui->lightVal->setText(QString::number(updatedLightValue) + " lux");
}
