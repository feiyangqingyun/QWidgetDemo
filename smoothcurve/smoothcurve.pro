QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = smoothcurve
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmsmoothcurve.cpp
SOURCES     += smoothcurve.cpp

HEADERS     += frmsmoothcurve.h
HEADERS     += smoothcurve.h

FORMS       += frmsmoothcurve.ui
