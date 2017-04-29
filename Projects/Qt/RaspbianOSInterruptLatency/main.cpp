#include <QCoreApplication>

#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InterruptHandler itHndl;

    return a.exec();
}

InterruptHandler::InterruptHandler(QObject *parent) : QObject(parent)
{
    // In
    this->INPin.Init(GPIOIN, GPIO_In);
    this->INPin.WatchEdge(GPIO_EdgeDetectEnabled);

    // Out
    this->OUTPin.Init(GPIOOUT, GPIO_Out);
    this->OUTPin.Write(this->INPin.Read());

    connect(&this->INPin, SIGNAL(EdgeDetected(int)), this, SLOT(InValueChange(int)));
}

InterruptHandler::~InterruptHandler()
{
    disconnect(&this->INPin, SIGNAL(EdgeDetected(int)), this, SLOT(InValueChange(int)));
}

void InterruptHandler::InValueChange(int GPIOState)
{
    // In értékének kitétele az Out lábra
    this->OUTPin.Write(GPIOState);
}
