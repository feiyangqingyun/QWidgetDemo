TARGET      = friedberg
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    plot.h \
    friedberg2007.h

SOURCES = \
    friedberg2007.cpp \
    plot.cpp \
    main.cpp

include     ($$PWD/../../qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../qwt
