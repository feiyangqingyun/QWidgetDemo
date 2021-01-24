HEADERS += $$PWD/app.h
HEADERS += $$PWD/quiwidget.h
HEADERS += $$PWD/tcpclient.h
HEADERS += $$PWD/tcpserver.h

SOURCES += $$PWD/app.cpp
SOURCES += $$PWD/quiwidget.cpp
SOURCES += $$PWD/tcpclient.cpp
SOURCES += $$PWD/tcpserver.cpp

contains(DEFINES, websocket) {
HEADERS += $$PWD/webclient.h
HEADERS += $$PWD/webserver.h

SOURCES += $$PWD/webclient.cpp
SOURCES += $$PWD/webserver.cpp
}
