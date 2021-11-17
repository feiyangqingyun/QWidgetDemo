#默认假定采用webkit,因为Qt4没有qtHaveModule函数而且默认采用的是webkit
#如果在windows上没有浏览器模块则默认采用miniblink
DEFINES += webkit

#判断当前qt版本号
message($$QT_ARCH : $$QT_VERSION -> $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION)

#5.0以上版本
!lessThan(QT_MAJOR_VERSION, 5) {
#如果没有安装webkit模块
!qtHaveModule(webkit) {
DEFINES -= webkit
}
#如果安装了webenginewidgets模块
qtHaveModule(webenginewidgets) {
DEFINES += webengine
}
}

#6.0以上版本 强制使用miniblink
greaterThan(QT_MAJOR_VERSION, 5) {
#DEFINES -= webkit
#DEFINES += webminiblink
}

contains(DEFINES, webkit) {
QT += webkit
greaterThan(QT_MAJOR_VERSION, 4) {
QT += webkitwidgets
}
} else {
contains(DEFINES, webengine) {
QT += webenginewidgets
} else {
message("没有找到浏览器模块,将采用第三方浏览器模块哦")
#如果采用IE浏览器则改成 DEFINES += webie
#如果采用miniblink改成 DEFINES += webminiblink  只支持win
win32 {DEFINES += webminiblink}
greaterThan(QT_MAJOR_VERSION, 4) {
lessThan(QT_MAJOR_VERSION, 6) {
win32 {QT += axcontainer}
}
} else {
win32 {CONFIG += qaxcontainer}
}}}

#在win上也可以手动强制使用 miniblink 就算存在 webkit 或者 webengine
#DEFINES -= webkit
#DEFINES -= webengine
#DEFINES += webminiblink

