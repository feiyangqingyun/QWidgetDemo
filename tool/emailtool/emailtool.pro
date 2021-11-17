QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = emailtool
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmemailtool.cpp sendemailthread.cpp
HEADERS     += frmemailtool.h sendemailthread.h
FORMS       += frmemailtool.ui

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../3rd_smtpclient
include ($$PWD/../3rd_smtpclient/3rd_smtpclient.pri)
