QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = hotkey
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmhotkey.cpp
HEADERS     += frmhotkey.h
FORMS       += frmhotkey.ui

INCLUDEPATH += $$PWD/../3rd_qhotkey
include($$PWD/../3rd_qhotkey/3rd_qhotkey.pri)
