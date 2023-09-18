TARGET      = realtime
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    mainwindow.h \
    scrollzoomer.h \
    scrollbar.h \
    incrementalplot.h \
    randomplot.h

SOURCES = \
    main.cpp \
    mainwindow.cpp \
    scrollzoomer.cpp \
    scrollbar.cpp \
    incrementalplot.cpp \
    randomplot.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
