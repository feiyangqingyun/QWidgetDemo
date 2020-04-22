QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = ffmpegdemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

#ffmpeg3表示用ffmpeg3版本 可以改成ffmpeg4表示用ffmpeg4版本
#qt32表示采用32位的ffmpeg 可以改成qt64表示采用64位的ffmpeg
DEFINES     += ffmpeg3 qt32
SOURCES     += main.cpp widget.cpp
HEADERS     += widget.h
FORMS       += widget.ui
CONFIG      += warn_off

INCLUDEPATH += $$PWD/ffmpeg
include ($$PWD/ffmpeg/ffmpeg.pri)
