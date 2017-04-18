#-------------------------------------------------
#
# Project created by QtCreator 2017-04-16T23:10:48
#
#-------------------------------------------------

QT       += core gui\
            serialport\
            concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = RaspbianOSTest
TEMPLATE = app

LIBS += -lwiringPi

SOURCES += main.cpp\
        mainwindow.cpp \
    pigpio.cpp \
    bglib.cpp

HEADERS  += mainwindow.h \
    pigpio.h \
    config.h \
    bglib.h

FORMS    += mainwindow.ui
