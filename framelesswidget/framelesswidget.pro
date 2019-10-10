#-------------------------------------------------
#
# Project created by QtCreator 2019-10-03T10:55:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = framelesswidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmframelesswidget.cpp
SOURCES     += framelesswidget.cpp

HEADERS     += frmframelesswidget.h
HEADERS     += framelesswidget.h

FORMS       += frmframelesswidget.ui
