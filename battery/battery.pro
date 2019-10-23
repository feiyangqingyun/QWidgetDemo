#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T09:46:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = battery
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmbattery.cpp
SOURCES     += battery.cpp

HEADERS     += frmbattery.h
HEADERS     += battery.h

FORMS       += frmbattery.ui
