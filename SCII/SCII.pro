#-------------------------------------------------
#
# Project created by QtCreator 2011-06-13T14:11:19
#
#-------------------------------------------------

QT       += core gui

TARGET = SCII
TEMPLATE = app


SOURCES += main.cpp\
        sciimainwindow.cpp \
    labjack.cpp \
    speed.cpp

HEADERS  += sciimainwindow.h \
    labjack.h \
    LJUD_DynamicLinking.h \
    LabJackUD.h \
    speed.h

FORMS    += sciimainwindow.ui

OTHER_FILES += \
    README_SCII.txt
