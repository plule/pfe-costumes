INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

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
SOURCES += $$PWD/dcrawqt.cpp \
    $$PWD/dcraw.cpp \
    $$PWD/dcrawqtoptions.cpp \
    $$PWD/dcrimage.cpp
HEADERS += $$PWD/dcrawqt.h \
    $$PWD/dcraw.h \
    $$PWD/dcrawqtoptions.h \
    $$PWD/dcrawqt_global.h \
    $$PWD/dcrimage.h
FORMS += $$PWD/dcrawqtoptions.ui
