DEFINES += BUILD_QXT_CORE BUILD_QXT_GUI

HEADERS += $$PWD/qxtglobal.h
HEADERS += $$PWD/qxtglobalshortcut_p.h
HEADERS += $$PWD/qxtglobalshortcut.h

SOURCES += $$PWD/qxtglobal.cpp
SOURCES += $$PWD/qxtglobalshortcut.cpp

unix:!macx {
CONFIG += X11
QT += x11extras
HEADERS += $$PWD/x11info.h
SOURCES += $$PWD/qxtwindowsystem_x11.cpp
SOURCES += $$PWD/qxtglobalshortcut_x11.cpp
}

macx {
QMAKE_LFLAGS += -framework Carbon -framework CoreFoundation
HEADERS += $$PWD/qxtwindowsystem_mac.h
SOURCES += $$PWD/qxtwindowsystem_mac.cpp
SOURCES += $$PWD/qxtglobalshortcut_mac.cpp
}

win32 {
LIBS += -luser32
SOURCES += $$PWD/qxtwindowsystem_win.cpp
SOURCES += $$PWD/qxtglobalshortcut_win.cpp
}
