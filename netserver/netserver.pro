#-------------------------------------------------
#
# Project created by QtCreator 2018-09-19T13:33:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = netserver
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = bin
RC_FILE     = other/main.rc

SOURCES     += main.cpp
HEADERS     += head.h
RESOURCES   += other/main.qrc
CONFIG      += warn_off

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form
