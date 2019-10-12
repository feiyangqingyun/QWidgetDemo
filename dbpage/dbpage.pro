#-------------------------------------------------
#
# Project created by QtCreator 2017-01-15T14:34:50
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET              = dbpage
TEMPLATE            = app
MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin
PRECOMPILED_HEADER  = head.h

INCLUDEPATH         += $$PWD
CONFIG              += warn_off

SOURCES += \
    main.cpp \
    dbpage.cpp \
    frmdbpage.cpp

HEADERS += \
    dbpage.h \
    frmdbpage.h

FORMS += \
    frmdbpage.ui
