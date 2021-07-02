QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = miniblink
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
HEADERS     += widget.h
SOURCES     += main.cpp widget.cpp
FORMS       += widget.ui

INCLUDEPATH += $$PWD/miniblink
include ($$PWD/miniblink/miniblink.pri)
