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
    $$PWD/base64helper.h \
    $$PWD/iconhelper.h \
    $$PWD/quiconfig.h \
    $$PWD/quidateselect.h \
    $$PWD/quihead.h \
    $$PWD/quihelper.h \
    $$PWD/quiinputbox.h \
    $$PWD/quimessagebox.h \
    $$PWD/quistyle.h \
    $$PWD/quitipbox.h \
    $$PWD/quiwidget.h

SOURCES += \
    $$PWD/base64helper.cpp \
    $$PWD/iconhelper.cpp \
    $$PWD/quiconfig.cpp \
    $$PWD/quidateselect.cpp \
    $$PWD/quihelper.cpp \
    $$PWD/quiinputbox.cpp \
    $$PWD/quimessagebox.cpp \
    $$PWD/quistyle.cpp \
    $$PWD/quitipbox.cpp \
    $$PWD/quiwidget.cpp

RESOURCES += $$PWD/resource/qm.qrc
RESOURCES += $$PWD/resource/ttf.qrc
