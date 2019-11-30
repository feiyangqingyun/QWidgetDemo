#-------------------------------------------------
#
# Project created by QtCreator 2016-09-29T09:37:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET          = videopanel
TEMPLATE        = app
MOC_DIR         = temp/moc
RCC_DIR         = temp/rcc
UI_DIR          = temp/ui
OBJECTS_DIR     = temp/obj
DESTDIR         = bin

SOURCES         += main.cpp
SOURCES         += frmmain.cpp
HEADERS         += frmmain.h
FORMS           += frmmain.ui
RESOURCES       += main.qrc
CONFIG          += warn_off
