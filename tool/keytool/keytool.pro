QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = keytool
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmmain.cpp
HEADERS     += frmmain.h
FORMS       += frmmain.ui

