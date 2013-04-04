DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp cameraHandler/QCamera.cpp
HEADERS += cameraHandler/cameraHandler.h cameraHandler/QCamera.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port
