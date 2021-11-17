QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = videobox
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmvideobox.cpp
SOURCES     += videobox.cpp

HEADERS     += frmvideobox.h
HEADERS     += videobox.h

FORMS       += frmvideobox.ui
