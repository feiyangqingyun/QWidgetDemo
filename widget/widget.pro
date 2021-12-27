TEMPLATE = subdirs
SUBDIRS += colorwidget
SUBDIRS += framelesswidget
SUBDIRS += gifwidget
SUBDIRS += lunarcalendarwidget
SUBDIRS += maskwidget
SUBDIRS += movewidget
SUBDIRS += screenwidget

#Qt6有些项目还不支持
greaterThan(QT_MAJOR_VERSION, 5) {
msvc {
SUBDIRS -= lunarcalendarwidget
}
}
