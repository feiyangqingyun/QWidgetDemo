#指定编译产生的文件分门别类放到对应目录
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj

#指定编译生成的可执行文件放到源码上一级目录下的bin目录
!android {
!wasm {
DESTDIR = $$PWD/../bin
}}

#把所有警告都关掉眼不见为净
CONFIG += warn_off
#开启大资源支持
CONFIG += resources_big
#开启后会将打印信息用控制台输出
#CONFIG += console

#引入全志H3芯片依赖
include ($$PWD/h3.pri)

HEADERS += \
    $$PWD/appinit.h \
    $$PWD/iconhelper.h \
    $$PWD/quihelper.h

SOURCES += \
    $$PWD/appinit.cpp \
    $$PWD/iconhelper.cpp \
    $$PWD/quihelper.cpp

RESOURCES += \
    $$PWD/common.qrc
