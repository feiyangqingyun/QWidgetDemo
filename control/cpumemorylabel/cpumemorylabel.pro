QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = cpumemorylabel
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmcpumemorylabel.cpp
SOURCES     += cpumemorylabel.cpp

HEADERS     += frmcpumemorylabel.h
HEADERS     += cpumemorylabel.h

FORMS       += frmcpumemorylabel.ui
