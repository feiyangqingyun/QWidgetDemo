TARGET      = refreshtest
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    settings.h \
    circularbuffer.h \
    panel.h \
    plot.h \
    mainwindow.h

SOURCES = \
    circularbuffer.cpp \
    panel.cpp \
    plot.cpp \
    mainwindow.cpp \
    main.cpp

include     ($$PWD/../../qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../qwt
