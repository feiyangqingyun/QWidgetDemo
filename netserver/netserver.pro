#-------------------------------------------------
#
# Project created by QtCreator 2018-09-19T13:33:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = netserver
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
RC_FILE     = other/main.rc

SOURCES     += main.cpp
HEADERS     += head.h
RESOURCES   += other/main.qrc
CONFIG      += warn_off

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../core_qui
include ($$PWD/../core_qui/core_qui.pri)
