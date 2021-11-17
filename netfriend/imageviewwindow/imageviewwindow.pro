QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = imageviewwindow
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

HEADERS     += graphicsview.h
HEADERS     += graphicspixmap.h
HEADERS     += imageviewwindow.h

SOURCES     += main.cpp
SOURCES     += graphicspixmap.cpp
SOURCES     += graphicsview.cpp
SOURCES     += imageviewwindow.cpp

FORMS       += imageviewwindow.ui
RESOURCES   += imageviewwindow.qrc
