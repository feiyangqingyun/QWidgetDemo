TARGET      = controls
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    sliderbox.h \
    slidertab.h \
    wheelbox.h \
    wheeltab.h \
    knobbox.h \
    knobtab.h \
    dialbox.h \
    dialtab.h

SOURCES = \
    sliderbox.cpp \
    slidertab.cpp \
    wheelbox.cpp \
    wheeltab.cpp \
    knobbox.cpp \
    knobtab.cpp \
    dialbox.cpp \
    dialtab.cpp \
    main.cpp

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
