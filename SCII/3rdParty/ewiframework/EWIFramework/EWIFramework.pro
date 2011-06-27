# Copyright (c) 2009 Roberto Estrada Casarrubios
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# 3. Neither the name of copyright holders nor the names of its
# contributors may be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
TEMPLATE = lib
TARGET = ewiframework
include(EWIFramework.pri)
include(../third_party/common/common.pri)
include(../third_party/5waybutton/5waybutton.pri)
# include(../third_party/basicdialgauge/basicdialgauge.pri)
include(../third_party/basicgraph/basicgraph.pri)
include(../third_party/multislider/multislider.pri)
include(../third_party/scrolldial/scrolldial.pri)
# include(../third_party/scrollwheel/scrollwheel.pri)
include(../third_party/svgbutton/svgbutton.pri)
include(../third_party/svgdialgauge/svgdialgauge.pri)
include(../third_party/svgslideswitch/svgslideswitch.pri)
include(../third_party/svgtoggleswitch/svgtoggleswitch.pri)
QT += xml \
    svg
CONFIG += designer \
    plugin \
    release
CONFIG += release
CONFIG += create_prl
CONFIG += link_prl
DESTDIR = ../build/release
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/release \
    .
RESOURCES += ../third_party/skins/thermometer_svgdialgauge.qrc \
    ../third_party/skins/tachometer_svgdialgauge.qrc \
    ../third_party/skins/amperemeter_svgdialgauge.qrc \
    ../third_party/skins/beryl_5waybutton.qrc \
    ../third_party/skins/beryl_multislider.qrc \
    ../third_party/skins/beryl_svgslideswitch.qrc \
    ../third_party/skins/beryl_svgbutton.qrc \
    ../third_party/skins/beryl_svgtoggleswitch.qrc \
    ../third_party/skins/berylsquare_svgtoggleswitch.qrc \
    ../third_party/skins/berylsquare_svgbutton.qrc \
    ../third_party/skins/beryl_scrollwheel.qrc \
    ../third_party/skins/beryl_scrolldial.qrc \
    ../third_party/skins/metallicbrush_svgbutton.qrc \
    ../third_party/skins/metallicbrush_svgslideswitch.qrc \
    ../third_party/iuskins/iuskins.qrc
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
DEFINES += QT_LARGEFILE_SUPPORT \
    QT_XML_LIB \
    QT_NODEBUG
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

# Install the plugin in the designer plugins directory.
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
