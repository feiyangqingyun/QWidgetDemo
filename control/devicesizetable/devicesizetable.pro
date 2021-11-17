QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = devicesizetable
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmdevicesizetable.cpp
SOURCES     += devicesizetable.cpp

HEADERS     += frmdevicesizetable.h
HEADERS     += devicesizetable.h

FORMS       += frmdevicesizetable.ui
