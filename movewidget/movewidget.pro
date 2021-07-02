QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = movewidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmmovewidget.cpp
SOURCES     += movewidget.cpp

HEADERS     += frmmovewidget.h
HEADERS     += movewidget.h

FORMS       += frmmovewidget.ui
