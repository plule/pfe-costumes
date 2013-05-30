TEMPLATE = subdirs

SUBDIRS = 3rdparty/DcRawQT

CONFIG += ordered

SUBDIRS += core

core.depends = 3rdparty/DcRawQT
