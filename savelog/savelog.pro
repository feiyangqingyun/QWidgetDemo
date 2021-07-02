QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = savelog
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmsavelog.cpp
SOURCES     += savelog.cpp

HEADERS     += frmsavelog.h
HEADERS     += savelog.h

FORMS       += frmsavelog.ui
