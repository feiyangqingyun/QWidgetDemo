QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = gifwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmgifwidget.cpp
SOURCES     += gifwidget.cpp

HEADERS     += gifwidget.h
HEADERS     += frmgifwidget.h
HEADERS     += gif.h

FORMS       += frmgifwidget.ui
RESOURCES   += main.qrc
