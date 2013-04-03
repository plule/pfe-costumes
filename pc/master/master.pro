DESTDIR = ../bin
INCLUDEPATH += ..
VPATH = ..

SOURCES += master/main.cpp cameraHandler/cameraHandler.cpp
HEADERS += cameraHandler/cameraHandler.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port
