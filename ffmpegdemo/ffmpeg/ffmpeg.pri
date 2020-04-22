HEADERS     += $$PWD/ffmpeghead.h
HEADERS     += $$PWD/ffmpeg.h
SOURCES     += $$PWD/ffmpeg.cpp

#ffmpeg4则使用ffmpeg4的目录
contains(DEFINES, ffmpeg4) {
strPath = ffmpeg4
} else {
strPath = ffmpeg3
}

#32位的库则使用32位的库的头文件和lib文件
contains(DEFINES, qt32) {
strLib = winlib
strInclude = include
} else {
strLib = winlib64
strInclude = include64
}

INCLUDEPATH += $$PWD/$$strPath/$$strInclude
INCLUDEPATH += $$PWD/$$strPath/$$strInclude/plugins

win32 {
LIBS += -L$$PWD/$$strPath/$$strLib/ -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice
}

#请自行替换
unix {
LIBS += -L$$PWD/linuxlib/ -lavfilter -lavformat -lavdevice -lavcodec -lswscale -lavutil -lswresample -lavdevice -lpthread -lm -lz -lrt -ldl
}
