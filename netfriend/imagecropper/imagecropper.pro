QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off
CONFIG      += c++11

SOURCES += \
    base/imagecropperlabel.cpp \
    example/imagecropperdemo.cpp \
    example/main.cpp \
    example/mainwindow.cpp

HEADERS += \
    base/imagecropperdialog.h \
    base/imagecropperlabel.h \
    example/imagecropperdemo.h \
    example/mainwindow.h
