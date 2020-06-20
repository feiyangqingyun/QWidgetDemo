QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = ffmpegdemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

SOURCES     += main.cpp widget.cpp
HEADERS     += widget.h
FORMS       += widget.ui
CONFIG      += warn_off

INCLUDEPATH += $$PWD/ffmpeg
include ($$PWD/ffmpeg/ffmpeg.pri)
