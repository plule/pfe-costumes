DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp cameraHandler/QCamera.cpp common/SlotLog.cpp \	
    mainwindow.cpp \
    qphotodisplayer.cpp \
    ../cameraHandler/cameraexception.cpp \
    qturntable_sav.cpp \
    ../qturntable.cpp
HEADERS += cameraHandler/cameraHandler.h cameraHandler/QCamera.h common/SlotLog.h \
    mainwindow.h \
    qphotodisplayer.h \
    ../cameraHandler/cameraexception.h \
    qturntable_sav.h \
    ../qturntable.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui

TRANSLATIONS = master_fr.ts
