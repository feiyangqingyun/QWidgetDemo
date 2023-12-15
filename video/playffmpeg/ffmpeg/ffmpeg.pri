#如果用的是ffmpeg4内核请将ffmpeg3改成ffmpeg4
DEFINES += ffmpeg4
contains(DEFINES, ffmpeg4) {
path_main = ffmpeg4
} else {
path_main = ffmpeg3
}

contains(QT_ARCH, x86_64) {
path_lib = libwin64
} else {
path_lib = libwin32
}

#包含头文件
INCLUDEPATH += $$PWD/$$path_main/include
#链接库文件
LIBS += -L$$PWD/$$path_main/$$path_lib/ -lavformat -lavfilter -lavcodec -lswresample -lswscale -lavutil

HEADERS += $$PWD/ffmpeginclude.h
HEADERS += $$PWD/ffmpeg.h
SOURCES += $$PWD/ffmpeg.cpp
