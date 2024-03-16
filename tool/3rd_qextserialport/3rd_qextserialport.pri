#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

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
