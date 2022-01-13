QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
#判断是否有websocket模块
qtHaveModule(websockets) {
QT += websockets
DEFINES += websocket
}}

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = nettool
TEMPLATE    = app
RC_FILE     = qrc/main.rc
wasm {
RESOURCES   += qrc/font.qrc
} else {
DESTDIR     = $$PWD/../bin
}

HEADERS     += head.h
SOURCES     += main.cpp
RESOURCES   += qrc/main.qrc
CONFIG      += warn_off

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)
