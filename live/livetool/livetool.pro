#-------------------------------------------------
#
# Project created by QtCreator 2019-02-15T14:34:50
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = livetool
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp frmmain.cpp trayicon.cpp app.cpp
HEADERS     += frmmain.h trayicon.h app.h
FORMS       += frmmain.ui
RESOURCES   += main.qrc

