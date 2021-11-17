QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = shortcut
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmshortcut.cpp
HEADERS     += frmshortcut.h
FORMS       += frmshortcut.ui

INCLUDEPATH += $$PWD/../3rd_qxtglobalshortcut
include($$PWD/../3rd_qxtglobalshortcut/3rd_qxtglobalshortcut.pri)
