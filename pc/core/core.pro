#DESTDIR = ../bin
TEMPLATE = app
INCLUDEPATH += includes common ../3rdparty/DcRawQT ../3rdparty/qextserialport
VPATH = ..

QT += widgets sql

QMAKE_CXXFLAGS_DEBUG +=

QMAKE_LFLAGS_DEBUG +=

SOURCES += main.cpp \
    widgets/qboundedslider.cpp \
    widgets/qlinkedboundedsliders.cpp \
    widgets/qslidergroup.cpp \
    widgets/qdressformvisu.cpp
TARGET = ../mannequinage-commerce
target.path = /usr/bin
desktop.path = /usr/share/applications
desktop.files += mannequinage.desktop
desktop.files += mannequinage-visu.desktop
INSTALLS += target desktop

SOURCES += camerahandler/camerahandler.cpp \
    camerahandler/qcamera.cpp \
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
    widgets/qimagepreviewwindow.cpp \
    common/exifpixmap.cpp \
    mainwindow.cpp

HEADERS += camerahandler/camerahandler.h \
    camerahandler/qcamera.h \
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
    widgets/qimagepreviewwindow.h \
    common/exifpixmap.h \
    mainwindow.h \
    widgets/qboundedslider.h \
    widgets/qlinkedboundedsliders.h \
    widgets/qslidergroup.h \
    widgets/qdressformvisu.h

CONFIG += c++11 qt warn_on release debug

DEFINES += DCRAWQT_LIBRARY \
    __MINGW__ \
    USE_CIMG \
    NODEPS

LIBS += -lgphoto2 -lgphoto2_port -lexif ../3rdparty/DcRawQT/libDcRawQT.a ../3rdparty/qextserialport/libqextserialport.a

FORMS += \
    settingsform.ui \
    widgets/qellipseslider.ui \
    widgets/qimagepreviewwindow.ui \
    mainwindow.ui \
    widgets/qboundedslider.ui \
    widgets/qlinkedboundedsliders.ui \
    widgets/qslidergroup.ui \
    widgets/qdressformvisu.ui

TRANSLATIONS = resources/core_fr.ts

RESOURCES += \
    resources/translations.qrc \
    resources/pics/pics.qrc \
    resources/default-db.qrc \
    resources/icons/icons.qrc

OTHER_FILES += \
    core_fr.qm
