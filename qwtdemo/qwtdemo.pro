TARGET      = qwtdemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = bin

CONFIG      += warn_off
SOURCES     += main.cpp
SOURCES     += frmmain.cpp
FORMS       += frmmain.ui
HEADERS     += frmmain.h

include     ($$PWD/qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/qwt
