QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = uidemo10
TEMPLATE    = app

HEADERS     += head.h
SOURCES     += main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/form
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../core_base
include ($$PWD/../core_base/core_base.pri)
