#-------------------------------------------------
#
# Project created by QtCreator 2018-07-04T09:39:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = buttondefence
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmbuttondefence.cpp
SOURCES     += buttondefence.cpp

HEADERS     += frmbuttondefence.h
HEADERS     += buttondefence.h

FORMS       += frmbuttondefence.ui

RESOURCES   += main.qrc
