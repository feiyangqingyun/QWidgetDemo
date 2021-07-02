QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = screenwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmscreenwidget.cpp
SOURCES     += screenwidget.cpp

HEADERS     += screenwidget.h
HEADERS     += frmscreenwidget.h

FORMS       += frmscreenwidget.ui
