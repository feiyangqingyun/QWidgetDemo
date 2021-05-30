#-------------------------------------------------
#
# Project created by QtCreator 2019-02-16T15:08:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = ntpclient
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmntpclient.cpp
SOURCES     += ntpclient.cpp

HEADERS     += frmntpclient.h
HEADERS     += ntpclient.h

FORMS       += frmntpclient.ui
