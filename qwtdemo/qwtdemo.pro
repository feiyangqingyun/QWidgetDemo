TARGET      = qwtdemo
TEMPLATE    = app
DESTDIR     = $$PWD/../bin

CONFIG      += warn_off
SOURCES     += main.cpp
SOURCES     += frmmain.cpp
FORMS       += frmmain.ui
HEADERS     += frmmain.h

include ($$PWD/qwt/qwt.pri)
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/qwt
