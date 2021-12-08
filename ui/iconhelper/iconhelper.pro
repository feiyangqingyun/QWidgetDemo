QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = iconhelper
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp

SOURCES     += frmiconhelper.cpp
SOURCES     += iconhelper.cpp
SOURCES     += panelwidget.cpp

HEADERS     += frmiconhelper.h
HEADERS     += iconhelper.h
HEADERS     += panelwidget.h

FORMS       += frmiconhelper.ui

INCLUDEPATH += $$PWD
RESOURCES   += main.qrc
