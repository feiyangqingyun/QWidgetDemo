QT += network
greaterThan(QT_MAJOR_VERSION, 4) {
lessThan(QT_MAJOR_VERSION, 6) {
android {QT += androidextras}
} else {
QT += core-private
}}

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
#开启后不会生成空的 debug release 目录
#CONFIG -= debug_and_release

#引入全志H3芯片依赖
include ($$PWD/h3.pri)
#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD

HEADERS += $$PWD/appdata.h
SOURCES += $$PWD/appdata.cpp

HEADERS += $$PWD/appinit.h
SOURCES += $$PWD/appinit.cpp

HEADERS += $$PWD/base64helper.h
SOURCES += $$PWD/base64helper.cpp

HEADERS += $$PWD/customstyle.h
SOURCES += $$PWD/customstyle.cpp

HEADERS += $$PWD/iconhelper.h
SOURCES += $$PWD/iconhelper.cpp

HEADERS += $$PWD/quihelper.h
SOURCES += $$PWD/quihelper.cpp

#可以指定不加载对应的资源文件
!contains(DEFINES, no_qrc_image) {
RESOURCES += $$PWD/qrc/image.qrc
}

!contains(DEFINES, no_qrc_qm) {
RESOURCES += $$PWD/qrc/qm.qrc
}

!contains(DEFINES, no_qrc_font) {
RESOURCES += $$PWD/qrc/font.qrc
}
