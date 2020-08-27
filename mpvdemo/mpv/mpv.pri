HEADERS += $$PWD/mpvhead.h
HEADERS += $$PWD/mpvtool.h
HEADERS += $$PWD/mpv.h
SOURCES += $$PWD/mpv.cpp

#表示64位的构建套件
contains(QT_ARCH, x86_64) {
strLib = winlib64
strInclude = include64
} else {
strLib = winlib
strInclude = include
}

INCLUDEPATH += $$PWD/$$strInclude
win32 {
LIBS += -L$$PWD/$$strLib/ -llibmpv
}

#需要自己改为对应目录下的库
unix:!macx {}
macx {}
