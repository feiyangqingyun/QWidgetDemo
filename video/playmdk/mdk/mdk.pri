DEFINES += mdkx
#下面表示Qt4.7及以下版本移除标记
lessThan(QT_MAJOR_VERSION, 5) {
lessThan(QT_MINOR_VERSION, 8) {
DEFINES -= mdkx
}}

#mdk组件最低支持msvc2013(msvc2013=12/msvc2015=14)
msvc {
lessThan(MSVC_VER, 12) {
DEFINES -= mdkx
}}

contains(DEFINES, mdkx) {
QT *= opengl
#Qt6单独提取出来了openglwidgets模块
greaterThan(QT_MAJOR_VERSION, 5) {
QT *= openglwidgets
}
win32 {
LIBS *= -lopengl32 -lGLU32
}

#需要启用c11标准(5.5及以下版本需要手动开启/6.0以后默认都开启了)
lessThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS += -std=gnu++11
}
lessThan(QT_MAJOR_VERSION, 6) {
lessThan(QT_MINOR_VERSION, 6) {
QMAKE_CXXFLAGS += -std=gnu++11 # c++0x / c++11 / gnu++11
}}

contains(QT_ARCH, x86_64) {
path_lib = libwin64
} else {
path_lib = libwin32
}

#包含头文件
INCLUDEPATH += $$PWD/include
#链接库文件
LIBS += -L$$PWD/$$path_lib/ -lmdk

HEADERS += $$PWD/mdkinclude.h
HEADERS += $$PWD/mdkwidget.h
SOURCES += $$PWD/mdkwidget.cpp
}
