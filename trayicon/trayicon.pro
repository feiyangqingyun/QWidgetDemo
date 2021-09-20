QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = trayicon
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmtrayicon.cpp
SOURCES     += trayicon.cpp

HEADERS     += frmtrayicon.h
HEADERS     += trayicon.h

FORMS       += frmtrayicon.ui
RESOURCES   += main.qrc

