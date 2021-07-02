QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = videopanel
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmvideopanel.cpp
SOURCES     += videopanel.cpp

HEADERS     += frmvideopanel.h
HEADERS     += videopanel.h

FORMS       += frmvideopanel.ui
