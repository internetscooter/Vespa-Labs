/*
 * Copyright (c) 2009 Roberto Estrada Casarrubios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Roberto Estrada Casarrubios.
 * 4. Neither the name of copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "qt5waybuttonplugin.h"

Qt5WayButtonPlugin::Qt5WayButtonPlugin(QObject *parent) : QObject(parent)
{
    initialized = false;
}

void Qt5WayButtonPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if(initialized)
    {
        return;
    }
    initialized = true;
}

bool Qt5WayButtonPlugin::isInitialized() const
{
    return initialized;
}

QWidget* Qt5WayButtonPlugin::createWidget(QWidget *parent)
{
    Qt5WayButton *button = new Qt5WayButton(parent);
    button->setSkin("Beryl");    
    return button;
}

QString Qt5WayButtonPlugin::name() const
{
    return "Qt5WayButton";
}

QString Qt5WayButtonPlugin::group() const
{
    return "Qt Embedded Widgets";
}

QIcon Qt5WayButtonPlugin::icon() const
{
    return QIcon();
}

QString Qt5WayButtonPlugin::toolTip() const
{
     return "An SVG 5 way button widget";
}

QString Qt5WayButtonPlugin::whatsThis() const
{
     return "An SVG 5 way button widget";
}

bool Qt5WayButtonPlugin::isContainer() const
{
     return false;
}

QString Qt5WayButtonPlugin::includeFile() const
{
     return "<Qt5WayButton>";
}

QString Qt5WayButtonPlugin::domXml() const
 {
     return "<ui language=\"c++\">\n"
            " <widget class=\"Qt5WayButton\" name=\"fiveWayButton\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "    <width>150</width>\n"
            "    <height>150</height>\n"
            "   </rect>\n"
            "  </property>\n"            
            "  <property name=\"toolTip\" >\n"
            "   <string>An SVG 5 way button widget.</string>\n"
            "  </property>\n"
            "  <property name=\"whatsThis\" >\n"
            "   <string>An SVG 5 way button widget.</string>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
 }
