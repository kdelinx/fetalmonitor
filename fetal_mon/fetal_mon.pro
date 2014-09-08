#-------------------------------------------------
#
# Project created by QtCreator 2014-09-02T09:00:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fetal_mon
TEMPLATE = app


SOURCES += main.cpp\
        mainx11.cpp

HEADERS  += mainx11.h

FORMS    += mainx11.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    img.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml

