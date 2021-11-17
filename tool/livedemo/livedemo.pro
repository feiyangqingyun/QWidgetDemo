QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = livedemo
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmmain.cpp applive.cpp
HEADERS     += frmmain.h applive.h
FORMS       += frmmain.ui

