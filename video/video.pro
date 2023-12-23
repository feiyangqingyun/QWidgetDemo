TEMPLATE = subdirs
SUBDIRS += videobox
SUBDIRS += videopanel
SUBDIRS += videowindow

win32 {
SUBDIRS += playffmpeg
SUBDIRS += playvlc
SUBDIRS += plaympv
}

#下面是为了判断是否能启用mdk项目
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

win32 {
contains(DEFINES, mdkx) {
SUBDIRS += playmdk
}}
