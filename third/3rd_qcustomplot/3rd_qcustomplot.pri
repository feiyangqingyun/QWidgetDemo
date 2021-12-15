greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
#lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

!contains(DEFINES, qcustomplot_v1_3) {
!contains(DEFINES, qcustomplot_v2_0) {
!contains(DEFINES, qcustomplot_v2_1) {
greaterThan(QT_MAJOR_VERSION, 4) {
DEFINES += qcustomplot_v2_1
} else {
DEFINES += qcustomplot_v2_0
}}}}

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
greaterThan(QT_MAJOR_VERSION, 5) {
HEADERS += $$PWD/v2_1_6/qcustomplot.h
} else {
HEADERS += $$PWD/v2_1/qcustomplot.h
}
SOURCES += $$PWD/v2_1/qcustomplot.cpp
}}
