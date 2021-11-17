TEMPLATE = subdirs
SUBDIRS += comtool
SUBDIRS += nettool
SUBDIRS += netserver

SUBDIRS += base64helper
SUBDIRS += countcode
SUBDIRS += emailtool
SUBDIRS += moneytool
SUBDIRS += pngtool

SUBDIRS += keytool
SUBDIRS += keydemo
SUBDIRS += livetool
SUBDIRS += livedemo

#Qt6有些代码没适配
greaterThan(QT_MAJOR_VERSION, 5) {
SUBDIRS -= emailtool
}
