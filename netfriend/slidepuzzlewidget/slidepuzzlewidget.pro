QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TEMPLATE    = app
TARGET      = slidepuzzlewidget
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

HEADERS     += frmslidepuzzlewidget.h
HEADERS     += puzzlewidget.h
HEADERS     += sliderpuzzlewidget.h

SOURCES     += main.cpp
SOURCES     += frmslidepuzzlewidget.cpp
SOURCES     += puzzlewidget.cpp
SOURCES     += sliderpuzzlewidget.cpp

FORMS       += frmslidepuzzlewidget.ui
FORMS       += sliderpuzzlewidget.ui
RESOURCES += frmslidepuzzlewidget.qrc


