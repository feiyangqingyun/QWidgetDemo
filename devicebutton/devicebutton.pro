#-------------------------------------------------
#
# Project created by QtCreator 2018-07-04T09:39:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = devicebutton
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmdevicebutton.cpp
SOURCES     += devicebutton.cpp

HEADERS     += frmdevicebutton.h
HEADERS     += devicebutton.h

FORMS       += frmdevicebutton.ui

RESOURCES   += main.qrc
