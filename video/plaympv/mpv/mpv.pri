#默认mpv2
!contains(DEFINES, mpv1) {
!contains(DEFINES, mpv2) {
DEFINES *= mpv2
}}

#区分主目录
contains(DEFINES, mpv2) {
path_main = mpv2
} else {
path_main = mpv1
}

contains(QT_ARCH, x86_64) {
path_lib = libwin64
} else {
path_lib = libwin32
}

#包含头文件
INCLUDEPATH += $$PWD/$$path_main/include
#链接库文件
LIBS += -L$$PWD/$$path_main/$$path_lib -llibmpv

HEADERS += $$PWD/mpvinclude.h
HEADERS += $$PWD/mpvtool.h
HEADERS += $$PWD/mpv.h
SOURCES += $$PWD/mpv.cpp
