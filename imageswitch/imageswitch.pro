QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = imageswitch
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmimageswitch.cpp
SOURCES     += imageswitch.cpp

HEADERS     += frmimageswitch.h
HEADERS     += imageswitch.h

FORMS       += frmimageswitch.ui

RESOURCES   += main.qrc
