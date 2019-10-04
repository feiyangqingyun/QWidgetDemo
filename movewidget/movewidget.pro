#-------------------------------------------------
#
# Project created by QtCreator 2019-09-28T15:04:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = lightbutton
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmmovewidget.cpp
SOURCES     += movewidget.cpp

HEADERS     += frmmovewidget.h
HEADERS     += movewidget.h

FORMS       += frmmovewidget.ui
