HEADERS += $$PWD/vlchead.h
HEADERS += $$PWD/vlc.h
SOURCES += $$PWD/vlc.cpp

#如果用的是vlc3内核请将vlc2改成vlc3,两种内核不兼容,头文件也不一样,建议用vlc2
DEFINES += vlc2

#vlc3则使用vlc3的目录

contains(DEFINES, vlc3) {
strPath = vlc3
} else {
strPath = vlc2
}

#表示64位的构建套件
contains(QT_ARCH, x86_64) {
strLib = winlib64
strInclude = include64
} else {
strLib = winlib
strInclude = include
}

INCLUDEPATH += $$PWD/$$strPath/$$strInclude
INCLUDEPATH += $$PWD/$$strPath/$$strInclude/plugins

win32 {
LIBS += -L$$PWD/$$strPath/$$strLib/ -llibvlc -llibvlccore
}

#需要自己改为对应目录下的库
unix:!macx {}
macx {}
