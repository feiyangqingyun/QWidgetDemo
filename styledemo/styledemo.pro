QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = styledemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
SOURCES     += main.cpp
HEADERS     += head.h
HEADERS     += frmmain.h
SOURCES     += frmmain.cpp
FORMS       += frmmain.ui
RESOURCES   += other/qss.qrc
RESOURCES   += other/main.qrc

INCLUDEPATH += $$PWD
