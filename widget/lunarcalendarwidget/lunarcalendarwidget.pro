QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET          = lunarcalendarwidget
TEMPLATE        = app
DESTDIR         = $$PWD/../bin
CONFIG          += warn_off
RESOURCES       += main.qrc

SOURCES         += main.cpp
SOURCES         += frmlunarcalendarwidget.cpp
SOURCES         += lunarcalendaritem.cpp
SOURCES         += lunarcalendarinfo.cpp
SOURCES         += lunarcalendarwidget.cpp

HEADERS         += frmlunarcalendarwidget.h
HEADERS         += lunarcalendaritem.h
HEADERS         += lunarcalendarinfo.h
HEADERS         += lunarcalendarwidget.h

FORMS           += frmlunarcalendarwidget.ui

INCLUDEPATH     += $$PWD
