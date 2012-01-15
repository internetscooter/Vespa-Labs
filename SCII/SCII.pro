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
    speed.cpp \
    ljstream.cpp

HEADERS  += sciimainwindow.h \
    labjack.h \
    LJUD_DynamicLinking.h \
    LabJackUD.h \
    speed.h \
    ljstream.h

FORMS    += sciimainwindow.ui

OTHER_FILES += \
    README_SCII.txt \
    code_snippets.txt

# Qt Embedded Widgets
include(../SCII/3rdParty/ewiframework/third_party/common/common.pri)
include(../SCII/3rdParty/ewiframework/third_party/5waybutton/5waybutton.pri)
# include(../SCII/3rdParty/ewiframework/third_party/basicdialgauge/basicdialgauge.pri)
include(../SCII/3rdParty/ewiframework/third_party/basicgraph/basicgraph.pri)
include(../SCII/3rdParty/ewiframework/third_party/multislider/multislider.pri)
include(../SCII/3rdParty/ewiframework/third_party/scrolldial/scrolldial.pri)
# include(../SCII/3rdParty/ewiframework/third_party/scrollwheel/scrollwheel.pri)
include(../SCII/3rdParty/ewiframework/third_party/svgbutton/svgbutton.pri)
include(../SCII/3rdParty/ewiframework/third_party/svgdialgauge/svgdialgauge.pri)
include(../SCII/3rdParty/ewiframework/third_party/svgslideswitch/svgslideswitch.pri)
include(../SCII/3rdParty/ewiframework/third_party/svgtoggleswitch/svgtoggleswitch.pri)

QT += xml \
    svg

RESOURCES += \
    3rdParty/ewiframework/third_party/skins/tachometer_svgdialgauge.qrc \
    3rdParty/ewiframework/third_party/skins/thermometer_svgdialgauge.qrc \
    3rdParty/ewiframework/third_party/skins/tachometer_svgdialgauge.qrc \
    3rdParty/ewiframework/third_party/skins/metallicbrush_svgslideswitch.qrc \
    3rdParty/ewiframework/third_party/skins/metallicbrush_svgbutton.qrc \
    3rdParty/ewiframework/third_party/skins/berylsquare_svgtoggleswitch.qrc \
    3rdParty/ewiframework/third_party/skins/berylsquare_svgbutton.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_svgtoggleswitch.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_svgslideswitch.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_svgbutton.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_scrollwheel.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_scrolldial.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_multislider.qrc \
    3rdParty/ewiframework/third_party/skins/beryl_5waybutton.qrc \
    3rdParty/ewiframework/third_party/skins/amperemeter_svgdialgauge.qrc

