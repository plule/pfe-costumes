#DESTDIR = ../bin
TEMPLATE = app
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
    ../common/collection.cpp \
    ../common/qloadedlistwidget.cpp \
    ../common/uniqueproxymodel.cpp
HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
    common/slotlog.h \
    mainwindow.h \
    ../cameraHandler/cameraexception.h \
    ../common/qturntable.h \
    ../common/collection.h \
    ../common/qloadedlistwidget.h \
    ../common/uniqueproxymodel.h

CONFIG += qt warn_on release

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui

TRANSLATIONS = ../resources/master_fr.ts

RESOURCES += \
    ../resources/translations.qrc \
    ../resources/pics/pics.qrc \
    ../resources/default-db.qrc
