QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = vlcdemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

#vlc2表示用vlc2版本 可以改成vlc3表示用vlc3版本
#qt32表示采用32位的vlc 可以改成qt64表示采用64位的vlc
DEFINES     += vlc2 qt32
SOURCES     += main.cpp widget.cpp
HEADERS     += widget.h
FORMS       += widget.ui
CONFIG      += warn_off

INCLUDEPATH += $$PWD/vlc
include ($$PWD/vlc/vlc.pri)
