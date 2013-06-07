TEMPLATE = subdirs

SUBDIRS = 3rdparty/DcRawQT \
          3rdparty/qextserialport

CONFIG += ordered

SUBDIRS += core

core.depends = 3rdparty/DcRawQT\
               3rdparty/qextserialport

#TRANSLATIONS = core/resources/core_fr.ts
OTHER_FILES += \
    core/core_fr.qm
