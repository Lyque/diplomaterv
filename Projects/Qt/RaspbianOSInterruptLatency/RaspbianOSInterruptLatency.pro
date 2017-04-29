#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T16:10:36
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = RaspbianOSInterruptLatency
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    pigpio.cpp

HEADERS += \
    pigpio.h \
    main.h \
    config.h
