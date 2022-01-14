QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = qcustomplotdemo
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
SOURCES     += main.cpp
RESOURCES   += qrc/main.qrc
wasm {
RESOURCES   += qrc/font.qrc
}

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/frmcustom
include ($$PWD/frmcustom/frmcustom.pri)

INCLUDEPATH += $$PWD/frmexample
include ($$PWD/frmexample/frmexample.pri)

INCLUDEPATH += $$PWD/frmexample2
include ($$PWD/frmexample2/frmexample2.pri)

INCLUDEPATH += $$PWD/../3rd_qcustomplot
include ($$PWD/../3rd_qcustomplot/3rd_qcustomplot.pri)
