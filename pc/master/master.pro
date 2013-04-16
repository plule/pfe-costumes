#DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp cameraHandler/QCamera.cpp common/SlotLog.cpp \	
    mainwindow.cpp \
    ../cameraHandler/cameraexception.cpp \
    ../qturntable.cpp
HEADERS += cameraHandler/cameraHandler.h cameraHandler/QCamera.h common/SlotLog.h \
    mainwindow.h \
    ../cameraHandler/cameraexception.h \
    ../qturntable.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui

TRANSLATIONS = master_fr.ts
