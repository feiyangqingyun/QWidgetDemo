QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = livetool
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp frmmain.cpp trayicon.cpp app.cpp
HEADERS     += frmmain.h trayicon.h app.h
FORMS       += frmmain.ui
RESOURCES   += main.qrc

