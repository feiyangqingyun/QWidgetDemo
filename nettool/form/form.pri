FORMS += $$PWD/frmmain.ui
FORMS += $$PWD/frmtcpclient.ui
FORMS += $$PWD/frmtcpserver.ui
FORMS += $$PWD/frmudpclient.ui
FORMS += $$PWD/frmudpserver.ui

HEADERS += $$PWD/frmmain.h
HEADERS += $$PWD/frmtcpclient.h
HEADERS += $$PWD/frmtcpserver.h
HEADERS += $$PWD/frmudpclient.h
HEADERS += $$PWD/frmudpserver.h

SOURCES += $$PWD/frmmain.cpp
SOURCES += $$PWD/frmtcpclient.cpp
SOURCES += $$PWD/frmtcpserver.cpp
SOURCES += $$PWD/frmudpclient.cpp
SOURCES += $$PWD/frmudpserver.cpp

contains(DEFINES, websocket) {
FORMS   += $$PWD/frmwebclient.ui
FORMS   += $$PWD/frmwebserver.ui

HEADERS += $$PWD/frmwebclient.h
HEADERS += $$PWD/frmwebserver.h

SOURCES += $$PWD/frmwebclient.cpp
SOURCES += $$PWD/frmwebserver.cpp
}
