#DESTDIR = ../bin
INCLUDEPATH += .. ../common
VPATH = ..

QT += widgets sql

SOURCES += master/main.cpp \
    camerahandler/camerahandler.cpp \
    camerahandler/qcamera.cpp \
    common/slotlog.cpp \
    mainwindow.cpp \
    ../camerahandler/cameraexception.cpp \
    ../common/qturntable.cpp \
    ../common/costume.cpp \
    ../common/collectionmanager.cpp
HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
    common/slotlog.h \
    mainwindow.h \
    ../cameraHandler/cameraexception.h \
    ../common/qturntable.h \
    ../common/costume.h \
    ../common/collectionmanager.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui

TRANSLATIONS = master_fr.ts

RESOURCES += \
    ../resources/pics/pics.qrc
