QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = frameless
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
HEADERS     += head.h
SOURCES     += main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/form
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../core_frameless
include ($$PWD/../core_frameless/core_frameless.pri)

INCLUDEPATH += $$PWD/../core_common
include ($$PWD/../core_common/core_common.pri)
