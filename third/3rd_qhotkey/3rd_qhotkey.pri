CONFIG += C++11
HEADERS += $$PWD/qhotkey.h
HEADERS += $$PWD/qhotkey_p.h
SOURCES += $$PWD/qhotkey.cpp

win32 {
LIBS += -luser32
SOURCES += $$PWD/qhotkey_win.cpp
}

unix:!macx {
QT += x11extras
LIBS += -lX11
SOURCES += $$PWD/qhotkey_x11.cpp
}

macx {
LIBS += -framework Carbon
SOURCES += $$PWD/qhotkey_mac.cpp
}
