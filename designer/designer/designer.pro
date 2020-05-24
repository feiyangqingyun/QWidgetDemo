#-------------------------------------------------
#
# Project created by QtCreator 2017-04-07T09:15:51
#
#-------------------------------------------------

QT                  += xml network
CONFIG              += warn_off release
RESOURCES           += designer.qrc
LIBS                += -lQtDesignerComponents -lQtDesigner

INCLUDEPATH         += $$PWD
INCLUDEPATH         += ../lib/sdk ../lib/extension ../lib/shared ../lib/uilib

TARGET              = designer
TEMPLATE            = app
MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin
TRANSLATIONS        = designer_zh_CN.ts
PRECOMPILED_HEADER  = qdesigner_pch.h
win32:RC_FILE       = main.rc

HEADERS += \
    qdesigner.h \
    qdesigner_toolwindow.h \
    qdesigner_formwindow.h \
    qdesigner_workbench.h \
    qdesigner_settings.h \
    qdesigner_actions.h \
    qdesigner_server.h \
    qdesigner_appearanceoptions.h \
    saveformastemplate.h \
    newform.h \
    versiondialog.h \
    designer_enums.h \
    appfontdialog.h \
    preferencesdialog.h \
    assistantclient.h \
    mainwindow.h \
    qttoolbardialog.h \
    fontpanel.h

SOURCES += main.cpp \
    qdesigner.cpp \
    qdesigner_toolwindow.cpp \
    qdesigner_formwindow.cpp \
    qdesigner_workbench.cpp \
    qdesigner_settings.cpp \
    qdesigner_server.cpp \
    qdesigner_actions.cpp \
    qdesigner_appearanceoptions.cpp \
    saveformastemplate.cpp \
    newform.cpp \
    versiondialog.cpp \
    appfontdialog.cpp \
    preferencesdialog.cpp \
    assistantclient.cpp \
    mainwindow.cpp \
    qttoolbardialog.cpp \
    fontpanel.cpp

FORMS += saveformastemplate.ui \
    preferencesdialog.ui \
    qdesigner_appearanceoptions.ui \
    qttoolbardialog.ui

