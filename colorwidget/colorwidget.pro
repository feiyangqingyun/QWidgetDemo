#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T22:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = colorwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmcolorwidget.cpp
SOURCES     += colorwidget.cpp

HEADERS     += colorwidget.h
HEADERS     += frmcolorwidget.h

FORMS       += frmcolorwidget.ui
