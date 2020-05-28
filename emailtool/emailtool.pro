#-------------------------------------------------
#
# Project created by QtCreator 2016-09-29T13:23:25
#
#-------------------------------------------------

QT       += core gui network sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = emailtool
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmemailtool.cpp
HEADERS     += frmemailtool.h
FORMS       += frmemailtool.ui

include ($$PWD/sendemail/sendemail.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/sendemail
