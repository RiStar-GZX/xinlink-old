TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=$$PWD/src
INCLUDEPATH +=$$PWD/include

SOURCES += \
    src/core.c \
    src/device.c \
    src/event.c \
    src/instruction.c \
    src/main.c \
    src/network.c

HEADERS += \
    include/core.h \
    include/device.h \
    include/event.h \
    include/instruction.h \
    include/network.h \
    include/type.h


