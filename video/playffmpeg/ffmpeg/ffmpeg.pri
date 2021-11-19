HEADERS += $$PWD/ffmpeghead.h
HEADERS += $$PWD/ffmpeg.h
SOURCES += $$PWD/ffmpeg.cpp

#如果用的是ffmpeg4内核请将ffmpeg3改成ffmpeg4,两种内核不兼容,头文件也不一样
DEFINES += ffmpeg4

#ffmpeg4则使用ffmpeg4的目录

contains(DEFINES, ffmpeg4) {
strPath = ffmpeg4
} else {
strPath = ffmpeg3
}

#表示64位的构建套件
contains(QT_ARCH, x86_64) {
strLib = winlib64
strInclude = include64
} else {
#由于Qt4不支持QT_ARCH所以会执行下面的
#如果用的64位的Qt4则需要自行修改

strLib = winlib
strInclude = include
}

#表示arm平台构建套件

contains(QT_ARCH, arm) {
strInclude = include
}

!android {
INCLUDEPATH += $$PWD/$$strPath/$$strInclude
}

win32 {
LIBS += -L$$PWD/$$strPath/$$strLib/ -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice
}

#请自行替换

!android {
unix:!macx {
LIBS += -L$$PWD/linuxlib/ -lavfilter -lavformat -lavdevice -lavcodec -lswscale -lavutil -lswresample -lavdevice -lpthread -lm -lz -lrt -ldl
}}

#android系统
android {
INCLUDEPATH += $$PWD/androidlib/include
LIBS += -L$$PWD/androidlib/ -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample
#将动态库文件一起打包
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libavcodec.so
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libavfilter.so
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libavformat.so
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libavutil.so
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libswresample.so
ANDROID_EXTRA_LIBS += $$PWD/androidlib/libswscale.so
}
