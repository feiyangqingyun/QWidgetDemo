TEMPLATE = subdirs
SUBDIRS += videobox
SUBDIRS += videopanel
SUBDIRS += videowidget

win32 {
SUBDIRS += playffmpeg
SUBDIRS += playvlc
SUBDIRS += plaympv
}
