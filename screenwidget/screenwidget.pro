#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T22:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = screenwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmscreenwidget.cpp
SOURCES     += screenwidget.cpp

HEADERS     += screenwidget.h
HEADERS     += frmscreenwidget.h

FORMS       += frmscreenwidget.ui
