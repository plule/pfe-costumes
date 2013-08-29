#DESTDIR = ../bin
TEMPLATE = app
INCLUDEPATH += includes common ../3rdparty/DcRawQT ../3rdparty/qextserialport
VPATH = ..

QT += widgets sql

QMAKE_CXXFLAGS_DEBUG += -pg

QMAKE_LFLAGS_DEBUG += -pg

SOURCES += main.cpp \
    camerahandler/camerahandler.cpp \
    camerahandler/qcamera.cpp \
    mainwindow.cpp \
    common/collection.cpp \
    common/uniqueproxymodel.cpp \
    widgets/qloadedlistwidget.cpp \
    widgets/qturntable.cpp \
    masscapture.cpp \
    settingsform.cpp \
    common/transaction.cpp \
    widgets/qellipseslider.cpp \
    common/qlistmodel.cpp \
    common/arduinocommunication.cpp \
    widgets/qimagepreviewwindow.cpp

HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
    mainwindow.h \
    common/collection.h \
    common/uniqueproxymodel.h \
    includes/interfaces.h \
    includes/morphology.h \
    widgets/qturntable.h \
    widgets/qloadedlistwidget.h \
    masscapture.h \
    settingsform.h \
    common/transaction.h \
    widgets/qellipseslider.h \
    common/qlistmodel.h \
    common/arduinocommunication.h \
    includes/settings.h \
    widgets/qimagepreviewwindow.h

CONFIG += c++11 qt warn_on release debug

DEFINES += DCRAWQT_LIBRARY \
    __MINGW__ \
    USE_CIMG \
    NODEPS

LIBS += -lgphoto2 -lgphoto2_port -lexif ../3rdparty/DcRawQT/libDcRawQT.a ../3rdparty/qextserialport/libqextserialport.a

TARGET = ../mannequinage
target.path = /usr/bin
desktop.path = /usr/share/applications
desktop.files += mannequinage.desktop
INSTALLS += target desktop

FORMS += \
    mainwindow.ui \
    settingsform.ui \
    widgets/qellipseslider.ui \
    widgets/qimagepreviewwindow.ui

TRANSLATIONS = resources/core_fr.ts

RESOURCES += \
    resources/translations.qrc \
    resources/pics/pics.qrc \
    resources/default-db.qrc \
    resources/icons/icons.qrc

OTHER_FILES += \
    core_fr.qm
