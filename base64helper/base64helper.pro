QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = base64helper
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp

SOURCES     += frmbase64helper.cpp
SOURCES     += base64helper.cpp

HEADERS     += frmbase64helper.h
HEADERS     += base64helper.h

FORMS       += frmbase64helper.ui

