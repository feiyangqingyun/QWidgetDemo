QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = lightbutton
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmlightbutton.cpp
SOURCES     += lightbutton.cpp

HEADERS     += frmlightbutton.h
HEADERS     += lightbutton.h

FORMS       += frmlightbutton.ui
