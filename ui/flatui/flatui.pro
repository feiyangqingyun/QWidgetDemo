QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = flatui
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmflatui.cpp
SOURCES     += flatui.cpp

HEADERS     += frmflatui.h
HEADERS     += flatui.h

FORMS       += frmflatui.ui
