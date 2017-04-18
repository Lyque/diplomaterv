#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSemaphore>
#include <QQueue>
#include <QtConcurrent>
#include <QFuture>
#include <QFile>
#include <QDebug>

#include <wiringPiI2C.h>

#include "pigpio.h"
#include "bglib.h"

#include "config.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void initGPIO();
    void initI2C();
    void initBLE();

    void dumpToFile();
    void readADCPeriodically();
    void readBLEPeriodically();

    void processLocalTemp(int data);
    void processPotmeter(int data);

signals:
    void newLocalTempData(int data);
    void newPotmeterData(int data);

    void newAmbTempData(int data);
    void newRemTempData(int data);
    void newLightData(float data);

    void errorMessage(QString errorValue, QString errorMessage);

    void sendBLECommand(QByteArray command);

private slots:
    void SW0ValueChange(int GPIOState);
    void SW1ValueChange(int GPIOState);
    void InValueChange(int GPIOState);

    // BGLib Responses
    void HelloResponse(QObject *sender, BGLib::HelloEventArgs e);
    void GAPConnectDirectResponse(QObject *sender, BGLib::ConnectDirectEventArgs e);
    // BGLib Events
    void SystemBootEvent(QObject *sender, BGLib::BootEventArgs e);
    void ConnectionStatusEvent(QObject *sender, BGLib::ConnectionStatusEventArgs e);
    void ATTClientProcedureCompletedEvent(QObject *sender, BGLib::ProcedureCompletedEventArgs e);
    void ATTClientAttributeValueEvent(QObject *sender, BGLib::AttributeValueEventArgs e);

    // BLE serial data received
    void BLESerialDataReceived();

    void on_checkBoxLed0_toggled(bool checked);
    void on_checkBoxLed1_toggled(bool checked);

    void newLocalTempDataSlot(int data);
    void newPotmeterDataSlot(int data);

    void newAmbTempDataSlot(int data);
    void newRemTempDataSlot(int data);
    void newLightDataSlot(float data);

    void errorMessageReceived(QString errorValue, QString errorMessage);

    void sendBLECommandSlot(QByteArray command);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    PiGPIO LED0Pin;
    PiGPIO LED1Pin;
    PiGPIO SW0Pin;
    PiGPIO SW1Pin;
    PiGPIO INPin;
    PiGPIO OUTPin;

    PiGPIO BLERstPin;
    PiGPIO ADCRstPin;

    int i2cDevice;

    BGLib bglib;
    QSerialPort *bleSerial;
    char bleConnectionHndl;
    QSemaphore *bleSemaphore;

    QFile *logfile;
    QQueue<QByteArray> *stringQueue;

    bool logfileThreadAbort;
    QFuture<void> logfileThread;
    bool adcThreadAbort;
    QFuture<void> adcThread;
    bool bleThreadAbort;
    QFuture<void> bleThread;
};

#endif // MAINWINDOW_H
