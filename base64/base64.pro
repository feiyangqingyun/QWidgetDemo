QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = base64
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp

SOURCES     += frmbase64.cpp
SOURCES     += base64.cpp

HEADERS     += frmbase64.h
HEADERS     += base64.h

FORMS       += frmbase64.ui

