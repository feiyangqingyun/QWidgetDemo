TARGET      = cpuplot
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS =  \
    cpuplot.h \
    cpustat.h \
    cpupiemarker.h 

SOURCES = \
    cpuplot.cpp \
    cpustat.cpp \
    cpupiemarker.cpp 

include     ($$PWD/../../qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../qwt
