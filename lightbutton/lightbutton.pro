#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T15:04:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = lightbutton
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmlightbutton.cpp
SOURCES     += lightbutton.cpp

HEADERS     += frmlightbutton.h
HEADERS     += lightbutton.h

FORMS       += frmlightbutton.ui
