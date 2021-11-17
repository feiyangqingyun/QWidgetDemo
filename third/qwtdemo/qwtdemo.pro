TARGET      = qwtdemo
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
SOURCES     += main.cpp
SOURCES     += frmmain.cpp
FORMS       += frmmain.ui
HEADERS     += frmmain.h

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../3rd_qwt
include ($$PWD/../3rd_qwt/3rd_qwt.pri)
