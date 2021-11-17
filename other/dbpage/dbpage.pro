QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET      = dbpage
TEMPLATE    = app
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += dbpage.cpp
SOURCES     += frmdbpage.cpp

HEADERS     += dbpage.h
HEADERS     += frmdbpage.h
FORMS       += frmdbpage.ui

#自动拷贝文件
src_file = $$PWD/TA.db
dst_file = $$DESTDIR
win32 {
#转换斜杠 / 到 \\
src_file ~= s,/,\\,g
dst_file ~= s,/,\\,g
dst_file2 ~= s,/,\\,g
QMAKE_POST_LINK += copy $$src_file $$dst_file
}
unix {
QMAKE_POST_LINK += cp -r -f $$src_file $$dst_file
}
