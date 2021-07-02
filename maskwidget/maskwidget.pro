QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = maskwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += maskwidget.cpp
SOURCES     += frmmaskwidget.cpp

HEADERS     += maskwidget.h
HEADERS     += frmmaskwidget.h

FORMS       += frmmaskwidget.ui


