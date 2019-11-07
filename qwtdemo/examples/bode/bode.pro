TARGET      = bode
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

SOURCES += main.cpp
SOURCES += plot.cpp
SOURCES += mainwindow.cpp
HEADERS += plot.h
HEADERS += mainwindow.h
HEADERS += complexnumber.h
HEADERS += pixmaps.h

include     ($$PWD/../../qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../qwt
