#-------------------------------------------------
#
# Project created by QtCreator 2017-01-15T14:34:50
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = dbpage
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += dbpage.cpp
SOURCES     += frmdbpage.cpp

HEADERS     += dbpage.h
HEADERS     += frmdbpage.h
FORMS       += frmdbpage.ui

INCLUDEPATH += $$PWD
