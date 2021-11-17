QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = framelesswidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmframelesswidget.cpp
SOURCES     += framelesswidget2.cpp

HEADERS     += frmframelesswidget.h
HEADERS     += framelesswidget2.h

FORMS       += frmframelesswidget.ui

INCLUDEPATH += $$PWD/framelesscore
INCLUDEPATH += $$PWD/framelessform

include ($$PWD/framelesscore/framelesscore.pri)
include ($$PWD/framelessform/framelessform.pri)
