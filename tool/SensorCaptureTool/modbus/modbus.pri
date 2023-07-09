win32:LIBS += -lws2_32

HEADERS += \
    $$PWD/modbus-private.h \
    $$PWD/modbus-rtu-private.h \
    $$PWD/modbus-rtu.h \
    $$PWD/modbus-tcp-private.h \
    $$PWD/modbus-tcp.h \
    $$PWD/modbus-version.h \
    $$PWD/modbus-config.h \
    $$PWD/modbus.h

SOURCES += \
    $$PWD/modbus-data.c \
    $$PWD/modbus-rtu.c \
    $$PWD/modbus-tcp.c \
    $$PWD/modbus.c


