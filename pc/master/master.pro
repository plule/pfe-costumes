#DESTDIR = ../bin
TEMPLATE = app
INCLUDEPATH += .. ../common
VPATH = ..

include(../3rdparty/qextserialport/qextserialport.pri)
include(../3rdparty/DcRawQT/DcRawQT.pri)

QT += widgets sql

SOURCES += master/main.cpp \
    camerahandler/camerahandler.cpp \
    camerahandler/qcamera.cpp \
    common/slotlog.cpp \
    mainwindow.cpp \
    ../camerahandler/cameraexception.cpp \
    ../common/collection.cpp \
    ../common/uniqueproxymodel.cpp \
    ../common/messagewatcher.cpp \
    ../widgets/qloadedlistwidget.cpp \
    ../widgets/qturntable.cpp \
    ../common/communication.cpp \
    masscapture.cpp \
    settingsform.cpp

HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
    common/slotlog.h \
    mainwindow.h \
    ../cameraHandler/cameraexception.h \
    ../common/collection.h \
    ../common/uniqueproxymodel.h \
    ../../interfaces/interfaces.h \
    ../../interfaces/morphology.h \
    ../common/messagewatcher.h \
    ../widgets/qturntable.h \
    ../widgets/qloadedlistwidget.h \
    ../common/communication.h \
    masscapture.h \
    settingsform.h

CONFIG += c++11 qt warn_on release debug

LIBS += -lgphoto2 -lgphoto2_port

FORMS += \
    mainwindow.ui \
    settingsform.ui \

TRANSLATIONS = ../resources/master_fr.ts

RESOURCES += \
    ../resources/translations.qrc \
    ../resources/pics/pics.qrc \
    ../resources/default-db.qrc

OTHER_FILES += \
    master.pro.user \
    master_fr.qm
