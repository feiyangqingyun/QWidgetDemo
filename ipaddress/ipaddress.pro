QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

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
