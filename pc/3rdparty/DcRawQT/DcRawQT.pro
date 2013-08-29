# -------------------------------------------------
# Project created by QtCreator 2009-07-30T14:24:28
# -------------------------------------------------
 QT += core widgets gui
TARGET = DcRawQT
TEMPLATE = lib
CONFIG += staticlib
QMAKE_CXXFLAGS += -w # dcraw is fuuuuull of warnings...

DEFINES += DCRAWQT_LIBRARY \
    __MINGW__ \
    USE_CIMG \
    NODEPS
win32 { 
    DEFINES += WIN32 \
        DJGPP
    LIBS += -lws2_32
}
!win32 {
#    LIBS += -L/usr/local/lib -llcms
}
SOURCES += dcrawqt.cpp \
    dcraw.cpp \
    dcrawqtoptions.cpp \
    dcrimage.cpp
HEADERS += dcrawqt.h \
    dcraw.h \
    dcrawqtoptions.h \
    dcrawqt_global.h \
    dcrimage.h
FORMS += dcrawqtoptions.ui
