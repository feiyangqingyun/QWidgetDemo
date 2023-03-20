greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
#lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

#将当前目录加入到头文件路径
INCLUDEPATH += $$PWD
DEFINES += qcustomplot

#引入平滑曲线类
HEADERS += $$PWD/smoothcurve.h
SOURCES += $$PWD/smoothcurve.cpp

#没有定义任何版本则默认采用2.0
!contains(DEFINES, qcustomplot_v1_3) {
!contains(DEFINES, qcustomplot_v2_0) {
!contains(DEFINES, qcustomplot_v2_1) {
DEFINES += qcustomplot_v2_0
}}}

#定义了2.0版本在Qt5以上采用2.1
contains(DEFINES, qcustomplot_v2_0) {
!contains(DEFINES, qcustomplot_v2_1) {
greaterThan(QT_MAJOR_VERSION, 4) {
DEFINES -= qcustomplot_v1_3
DEFINES -= qcustomplot_v2_0
DEFINES += qcustomplot_v2_1
}}}

#根据定义的版本引入文件
contains(DEFINES, qcustomplot_v1_3) {
INCLUDEPATH += $$PWD/v1_3
HEADERS += $$PWD/v1_3/qcustomplot.h
SOURCES += $$PWD/v1_3/qcustomplot.cpp
} else {
contains(DEFINES, qcustomplot_v2_0) {
INCLUDEPATH += $$PWD/v2_0
HEADERS += $$PWD/v2_0/qcustomplot.h
SOURCES += $$PWD/v2_0/qcustomplot.cpp
} else {
INCLUDEPATH += $$PWD/v2_1
#引入对应修复不支持Qt6的头文件
greaterThan(QT_MAJOR_VERSION, 5) {
HEADERS += $$PWD/v2_1_6/qcustomplot.h
} else {
HEADERS += $$PWD/v2_1/qcustomplot.h
}
SOURCES += $$PWD/v2_1/qcustomplot.cpp
}}
