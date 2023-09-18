TARGET      = cpuplot
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS += cpuplot.h
HEADERS += cpustat.h
HEADERS += cpupiemarker.h

SOURCES += cpuplot.cpp
SOURCES += cpustat.cpp
SOURCES += cpupiemarker.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
