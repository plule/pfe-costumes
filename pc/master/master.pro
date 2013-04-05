DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp cameraHandler/QCamera.cpp common/SlotLog.cpp	
HEADERS += cameraHandler/cameraHandler.h cameraHandler/QCamera.h common/SlotLog.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port
