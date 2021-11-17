QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = uidemo08
TEMPLATE    = app

HEADERS     += head.h
SOURCES     += main.cpp
RESOURCES   += other/main.qrc
RESOURCES   += $$PWD/../core_qss/qss.qrc

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/form
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../core_common
include ($$PWD/../core_common/core_common.pri)
