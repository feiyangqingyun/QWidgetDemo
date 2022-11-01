TEMPLATE = subdirs
SUBDIRS += videobox
SUBDIRS += videopanel
SUBDIRS += videowindow

win32 {
SUBDIRS += playffmpeg
SUBDIRS += playvlc
SUBDIRS += plaympv
}
