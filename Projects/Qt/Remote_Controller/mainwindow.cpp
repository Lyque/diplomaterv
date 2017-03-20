#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);
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

    ui->tempLocalBar->setValue(50);
    ui->tempRemoteBar->setValue(50);
    ui->potmeterDial->setValue(2047);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionSettings->setEnabled(true);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    initActionsConnections();

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &MainWindow::handleError);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
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

        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
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
    serial->write(data);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
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
    char value[4];

    if(checked)
    {
        value[0] = 0xFF;
        value[1] = 0xFF;
        value[2] = 0xFF;
        value[3] = 0xFF;
    }
    else
    {
        value[0] = 0x00;
        value[1] = 0x00;
        value[2] = 0x00;
        value[3] = 0x00;
    }

    message.append(value, 4);
    writeData(message);
}

void MainWindow::on_led1CheckBox_toggled(bool checked)
{
    QByteArray message = "led1    :";
    char value[4];

    if(checked)
    {
        value[0] = 0xFF;
        value[1] = 0xFF;
        value[2] = 0xFF;
        value[3] = 0xFF;
    }
    else
    {
        value[0] = 0x00;
        value[1] = 0x00;
        value[2] = 0x00;
        value[3] = 0x00;
    }

    message.append(value, 4);
    writeData(message);
}
