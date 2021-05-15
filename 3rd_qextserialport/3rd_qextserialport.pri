HEADERS += $$PWD/qextserialport.h
HEADERS += $$PWD/qextserialport_global.h
HEADERS += $$PWD/qextserialport_p.h
           
SOURCES += $$PWD/qextserialport.cpp
win32 {
SOURCES += $$PWD/qextserialport_win.cpp
}
unix {
SOURCES += $$PWD/qextserialport_unix.cpp
}
