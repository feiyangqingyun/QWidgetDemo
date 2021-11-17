TARGET      = legends
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    mainwindow.h \
    panel.h \
    settings.h \
    plot.h

SOURCES = \
    mainwindow.cpp \
    panel.cpp \
    plot.cpp \
    main.cpp

include     ($$PWD/../../qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../qwt
