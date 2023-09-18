TARGET      = bode
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS += mainwindow.h
HEADERS += complexnumber.h
HEADERS += pixmaps.h

SOURCES += main.cpp
SOURCES += plot.cpp
SOURCES += mainwindow.cpp
HEADERS += plot.h

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
