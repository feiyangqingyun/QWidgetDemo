TARGET      = event_filter
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin

HEADERS = \
    colorbar.h \
    scalepicker.h \
    canvaspicker.h \
    plot.h 

SOURCES = \
    colorbar.cpp \
    scalepicker.cpp \
    canvaspicker.cpp \
    plot.cpp \
    event_filter.cpp 

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../../../3rd_qwt
include ($$PWD/../../../3rd_qwt/3rd_qwt.pri)
