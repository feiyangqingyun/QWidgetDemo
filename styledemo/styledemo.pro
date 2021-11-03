QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = styledemo
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

SOURCES     += main.cpp
HEADERS     += head.h
HEADERS     += frmmain.h
SOURCES     += frmmain.cpp
FORMS       += frmmain.ui

RESOURCES   += qrc/qss.qrc
RESOURCES   += qrc/qm.qrc
RESOURCES   += qrc/font.qrc
RESOURCES   += qrc/main.qrc

INCLUDEPATH += $$PWD
