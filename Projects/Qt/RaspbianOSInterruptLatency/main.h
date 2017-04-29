#ifndef MAIN_H
#define MAIN_H

#include <QtCore>

#include "config.h"
#include "pigpio.h"

class InterruptHandler : QObject
{
    Q_OBJECT
public:
    InterruptHandler(QObject *parent = 0);
    ~InterruptHandler();

private slots:
    void InValueChange(int GPIOState);

private:
    PiGPIO INPin;
    PiGPIO OUTPin;
};

#endif // MAIN_H
