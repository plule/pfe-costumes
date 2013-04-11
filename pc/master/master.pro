DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp cameraHandler/QCamera.cpp common/SlotLog.cpp \	
    mainwindow.cpp \
    qphotodisplayer.cpp \
    qturntable.cpp \
    ../cameraHandler/cameraexception.cpp
HEADERS += cameraHandler/cameraHandler.h cameraHandler/QCamera.h common/SlotLog.h \
    mainwindow.h \
    qphotodisplayer.h \
    qturntable.h \
    ../cameraHandler/cameraexception.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui \
    qturntable.ui
