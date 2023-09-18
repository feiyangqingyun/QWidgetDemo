QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = SensorCaptureTool
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
RC_FILE     = qrc/main.rc

HEADERS     += head.h
SOURCES     += main.cpp
RESOURCES   += qrc/main.qrc
CONFIG      += warn_off

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form
INCLUDEPATH += $$PWD/modbus
INCLUDEPATH += $$PWD/frmscrollbar

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)
include ($$PWD/modbus/modbus.pri)
include ($$PWD/frmscrollbar/frmscrollbar.pri)

INCLUDEPATH += $$PWD/../3rd_qextserialport
include ($$PWD/../3rd_qextserialport/3rd_qextserialport.pri)

INCLUDEPATH += $$PWD/3rd_qcustomplot
include ($$PWD/3rd_qcustomplot/3rd_qcustomplot.pri)
