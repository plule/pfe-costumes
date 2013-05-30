TEMPLATE = subdirs

SUBDIRS = 3rdparty/DcRawQT \
          3rdparty/qextserialport

CONFIG += ordered

SUBDIRS += core

core.depends = 3rdparty/DcRawQT\
               3rdparty/qextserialport
