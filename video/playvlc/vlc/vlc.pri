#默认用vlc3
!contains(DEFINES, vlc2) {
!contains(DEFINES, vlc3) {
DEFINES *= vlc3
}}

#区分主目录
contains(DEFINES, vlc3) {
path_main = vlc3
} else {
path_main = vlc2
}

contains(QT_ARCH, x86_64) {
path_lib = libwin64
} else {
path_lib = libwin32
}

#包含头文件
INCLUDEPATH += $$PWD/$$path_main/include
INCLUDEPATH += $$PWD/$$path_main/include/plugins
#链接库文件
LIBS += -L$$PWD/$$path_main/$$path_lib -llibvlc -llibvlccore

HEADERS += $$PWD/vlcinclude.h
HEADERS += $$PWD/vlc.h
SOURCES += $$PWD/vlc.cpp
