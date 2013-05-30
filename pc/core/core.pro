#DESTDIR = ../bin
TEMPLATE = app
INCLUDEPATH += ../common ../3rdparty/DcRawQT ../3rdparty/qextserialport
VPATH = ..

QT += widgets sql

SOURCES += main.cpp \
    camerahandler/camerahandler.cpp \
    camerahandler/qcamera.cpp \
    common/slotlog.cpp \
    mainwindow.cpp \
    common/collection.cpp \
    common/uniqueproxymodel.cpp \
    widgets/qloadedlistwidget.cpp \
    widgets/qturntable.cpp \
    common/communication.cpp \
    masscapture.cpp \
    settingsform.cpp \
    common/transaction.cpp

HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
    common/slotlog.h \
    mainwindow.h \
    common/collection.h \
    common/uniqueproxymodel.h \
    ../../interfaces/interfaces.h \
    ../../interfaces/morphology.h \
    widgets/qturntable.h \
    widgets/qloadedlistwidget.h \
    common/communication.h \
    masscapture.h \
    settingsform.h \
    common/transaction.h

CONFIG += c++11 qt warn_on release debug

DEFINES += DCRAWQT_LIBRARY \
    __MINGW__ \
    USE_CIMG \
    NODEPS

LIBS += -lgphoto2 -lgphoto2_port ../3rdparty/DcRawQT/libDcRawQT.a ../3rdparty/qextserialport/libqextserialport.a

TARGET = ../appli

FORMS += \
    mainwindow.ui \
    settingsform.ui \

TRANSLATIONS = resources/core_fr.ts

RESOURCES += \
    resources/translations.qrc \
    resources/pics/pics.qrc \
    resources/default-db.qrc

OTHER_FILES += \
    core_fr.qm
