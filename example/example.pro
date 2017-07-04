TEMPLATE = app

QT = core
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

BIN_PATH = $$PWD/build/bin
OBJ_PATH = $$PWD/build/obj
GOTOC_PATH = $$PWD

include($$PWD/../qbus/qbus.pri)

DESTDIR = $$BIN_PATH
OBJECTS_DIR = $$OBJ_PATH
MOC_DIR = $$OBJECTS_DIR
OBJMOC = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR

INCLUDEPATH += \
    $$PWD/../qbus/include

#DEPENDPATH += \
#    $$GOTOC_PATH

#PRE_TARGETDEPS += \
#    $$GOTOC_PATH/gotoc.a

win32: {
    LIBS += -lwinmm -lWs2_32
}

SOURCES += \
    main.cpp
