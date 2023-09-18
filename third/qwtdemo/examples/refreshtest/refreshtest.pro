TARGET      = refreshtest
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS += settings.h
HEADERS += circularbuffer.h
HEADERS += panel.h
HEADERS += plot.h
HEADERS += mainwindow.h

SOURCES += circularbuffer.cpp
SOURCES += panel.cpp
SOURCES += plot.cpp
SOURCES += mainwindow.cpp
SOURCES += main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
