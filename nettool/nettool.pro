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
DEFINES     += emsdk
RESOURCES   += qrc/font.qrc
} else {
DESTDIR     = $$PWD/../bin
}

SOURCES     += main.cpp
HEADERS     += head.h
RESOURCES   += qrc/main.qrc

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../core_qui
include ($$PWD/../core_qui/core_qui.pri)
