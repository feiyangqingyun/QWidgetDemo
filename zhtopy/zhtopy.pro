QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = zhtopy
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmzhtopy.cpp
SOURCES     += zhtopy.cpp

HEADERS     += frmzhtopy.h
HEADERS     += zhtopy.h

FORMS       += frmzhtopy.ui

RESOURCES   += main.qrc
