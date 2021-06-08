
#-------------------------------------------------
#
# Project created by QtCreator 2021-02-19T09:21:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = astackwidget
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += astackwidget.cpp
HEADERS     += astackwidget.h

SOURCES     += frmastackwidget.cpp
HEADERS     += frmastackwidget.h
FORMS       += frmastackwidget.ui
