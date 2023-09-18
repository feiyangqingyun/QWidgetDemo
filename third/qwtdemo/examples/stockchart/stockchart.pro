TARGET      = stockchart
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS += legend.h
HEADERS += griditem.h
HEADERS += plot.h
HEADERS += quotefactory.h

SOURCES += legend.cpp
SOURCES += griditem.cpp
SOURCES += quotefactory.cpp
SOURCES += plot.cpp
SOURCES += main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
