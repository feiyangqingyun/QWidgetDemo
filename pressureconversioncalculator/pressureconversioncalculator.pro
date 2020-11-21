#-------------------------------------------------
#
# Project created by QtCreator 2020-04-23T17:38:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = pressureconversioncalculator
RC_ICONS   = other/logo.ico
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

RESOURCES   += other/main.qrc
RESOURCES   += other/qss.qrc
CONFIG      += warn_off

SOURCES     += main.cpp

include     ($$PWD/api/api.pri)
include     ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form
