QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = imageviewwindow
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off

HEADERS += graphicsview.h
HEADERS += graphicspixmap.h
HEADERS += imageviewwindow.h

SOURCES += graphicspixmap.cpp
SOURCES += graphicsview.cpp
SOURCES += imageviewwindow.cpp
SOURCES += main.cpp

FORMS   += imageviewwindow.ui

RESOURCES += imageviewwindow.qrc
