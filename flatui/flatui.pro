#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T09:29:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = flatui
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmflatui.cpp
SOURCES     += flatui.cpp

HEADERS     += frmflatui.h
HEADERS     += flatui.h

FORMS       += frmflatui.ui
