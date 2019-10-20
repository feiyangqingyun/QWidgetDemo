#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T22:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = maskwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += maskwidget.cpp
SOURCES     += frmmaskwidget.cpp

HEADERS     += maskwidget.h
HEADERS     += frmmaskwidget.h

FORMS       += frmmaskwidget.ui


