#ifndef PIGPIO_H
#define PIGPIO_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QThread>

enum PiGPIOState
{
    GPIO_Low = 0,
    GPIO_High = 1
};

enum PiGPIOEdge
{
    GPIO_RisingEdge = 0,
    GPIO_FallingEdge = 1
};

enum PiGPIODirection
{
    GPIO_In = 0,
    GPIO_Out = 1
};

enum PiGPIOEdgeDetectEnable
{
    GPIO_EdgeDetectDisabled = 0,
    GPIO_EdgeDetectEnabled = 1
};

class PiGPIO : public QObject
{
    Q_OBJECT
private:
    QFile *valueFile_p;
    QFile *directionFile_p;
    QFileSystemWatcher *edgeWatcher_p;

    int previousValue;
    QString directoryPath;

public:
    explicit PiGPIO(QObject *parent = 0);
    ~PiGPIO();

    int Init(int GPIOPin, int GPIODirection);
    int Read();
    int Write(int GPIOState);
    int WatchEdge(int enable);

signals:
    void EdgeDetected(int GPIOState);

public slots:
    void TimeOut(QString filePath);
};

#endif // PIGPIO_H
