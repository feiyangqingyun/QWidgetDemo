#-------------------------------------------------
#
# Project created by QtCreator 2019-10-28T13:44:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = lineeditnext
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += widget.cpp
HEADERS     += widget.h
FORMS       += widget.ui
