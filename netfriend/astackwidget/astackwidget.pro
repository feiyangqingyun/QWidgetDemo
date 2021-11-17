QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = astackwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += astackwidget.cpp
HEADERS     += astackwidget.h

SOURCES     += frmastackwidget.cpp
HEADERS     += frmastackwidget.h
FORMS       += frmastackwidget.ui
