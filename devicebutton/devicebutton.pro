QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

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
