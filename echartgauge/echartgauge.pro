#-------------------------------------------------
#
# Project created by QtCreator 2016-05-05T22:58:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4){
QT      += widgets
win32 {
msvc{
QT      += webenginewidgets
}
} else {
QT      += webenginewidgets
}
} else {
QT      += webkit
DEFINES += webkit
}

TARGET      = echartgauge
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = bin

SOURCES     += main.cpp widget.cpp
HEADERS     += widget.h
FORMS       += widget.ui
