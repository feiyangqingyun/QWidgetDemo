HEADERS     += $$PWD/vlchead.h
HEADERS     += $$PWD/vlc.h   
SOURCES     += $$PWD/vlc.cpp

#vlc3则使用vlc3的目录
contains(DEFINES, vlc3) {
strPath = vlc3
} else {
strPath = vlc2
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
LIBS += -L$$PWD/$$strPath/$$strLib/ -llibvlc -llibvlccore
}

#需要自己改为对应目录下的库
unix:!macx {}
macx {}
