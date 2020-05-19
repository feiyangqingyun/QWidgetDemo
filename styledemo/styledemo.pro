#-------------------------------------------------
#
# Project created by QtCreator 2017-02-19T12:55:42
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = styledemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
SOURCES     += main.cpp
SOURCES     += frmmain.cpp
HEADERS     += head.h
HEADERS     += frmmain.h
FORMS       += frmmain.ui
RESOURCES   += other/qss.qrc
RESOURCES   += other/main.qrc

INCLUDEPATH += $$PWD
