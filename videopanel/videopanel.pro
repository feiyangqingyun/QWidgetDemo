#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T22:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = videopanel
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmvideopanel.cpp
SOURCES     += videopanel.cpp

HEADERS     += frmvideopanel.h
HEADERS     += videopanel.h

FORMS       += frmvideopanel.ui
