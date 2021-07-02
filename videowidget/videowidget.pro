QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = videowidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmvideowidget.cpp
SOURCES     += videowidget.cpp

HEADERS     += frmvideowidget.h
HEADERS     += videowidget.h

FORMS       += frmvideowidget.ui
