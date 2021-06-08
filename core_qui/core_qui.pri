HEADERS += \
    $$PWD/iconhelper.h \
    $$PWD/quiconfig.h \
    $$PWD/quidateselect.h \
    $$PWD/quihead.h \
    $$PWD/quihelper.h \
    $$PWD/quiinputbox.h \
    $$PWD/quimessagebox.h \
    $$PWD/quistyle.h \
    $$PWD/quitipbox.h \
    $$PWD/quiwidget.h

SOURCES += \
    $$PWD/iconhelper.cpp \
    $$PWD/quiconfig.cpp \
    $$PWD/quidateselect.cpp \
    $$PWD/quihelper.cpp \
    $$PWD/quiinputbox.cpp \
    $$PWD/quimessagebox.cpp \
    $$PWD/quistyle.cpp \
    $$PWD/quitipbox.cpp \
    $$PWD/quiwidget.cpp

!contains(DEFINES, no_qui_qm) {
RESOURCES += $$PWD/source/qm.qrc
}

!contains(DEFINES, no_qui_ttf) {
RESOURCES += $$PWD/source/ttf.qrc
}
