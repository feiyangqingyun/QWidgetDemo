#-------------------------------------------------
#
# Project created by QtCreator 2017-08-10T17:11:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = ipaddress
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmipaddress.cpp
SOURCES     += ipaddress.cpp

HEADERS     += frmipaddress.h
HEADERS     += ipaddress.h

FORMS       += frmipaddress.ui
