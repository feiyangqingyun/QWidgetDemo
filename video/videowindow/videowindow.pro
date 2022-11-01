QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = videowindow
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmvideowindow.cpp
SOURCES     += videowindow.cpp

HEADERS     += frmvideowindow.h
HEADERS     += videowindow.h

FORMS       += frmvideowindow.ui

RESOURCES   += main.qrc
