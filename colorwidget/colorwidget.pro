QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = colorwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmcolorwidget.cpp
SOURCES     += colorwidget.cpp

HEADERS     += colorwidget.h
HEADERS     += frmcolorwidget.h

FORMS       += frmcolorwidget.ui
