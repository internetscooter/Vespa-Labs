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

#ifndef QT5WAYBUTTONPLUGIN_H
#define QT5WAYBUTTONPLUGIN_H

#include <QObject>
#include <QtDesigner/QtDesigner>
#include <Qt5WayButton>

/**
 * Wraps a Qt5WayButton to expose it to QTDesigner.
 * The Widget is exposed to QTDesigner via an standard interface defined by QDesignerCustomWidgetInterface,
 * This wrapper class implements such interface to expose the widget to the QTDesigner.
 *
 * @see QDesignerCustomWidgetInterface.
 * @author Roberto Estrada Casarrubios.
 */
class Qt5WayButtonPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    /**
     * Constructor for the widget wrapper
     * @param parent This param specifies the parent for the widget which is being created.
     */
    Qt5WayButtonPlugin(QObject *parent = 0);
    /**
     * Returns if this widget is a container widget
     * @return True if it is a container, False in any other case.
     */
    bool isContainer() const;
    /**
     * Returns if this widget is initialized
     * @return True if it is initialized, False in any other case.
     */
    bool isInitialized() const;
    /**
     * Returns the widget icon to be shown on QTDesigner
     * @return The icon to be shown on QTDesigner
     */
    QIcon icon() const;
    /**
     * Returns an XML string with the default properties for the Widget
     * property inspector set. These properties are default values such as dimensions
     * @return An XML string with the default properties for the Widget
     */
    QString domXml() const;
    /**
     * Returns the name of the category of widgets to which the widget belongs
     * @return An string with the name of the category of widgets to which the widget belongs.
     */
    QString group() const;
    /**
     * Returns the name of the include file where the widget is defined.
     * @return An string with the name of the include file where the widget is defined.
     */
    QString includeFile() const;
    /**
     * Returns the name of the wrapped widget class.
     * @return An string with the name of the wrapped widget class.
     */
    QString name() const;
    /**
     * Returns a brief description of the widget's functions to be shown as a tooltip
     * in QTDesigner
     * @return A brief description of the widget's functions
     */
    QString toolTip() const;
    /**
     * Returns a detailed description of the widget's functions to be shown as a tooltip
     * in QTDesigner
     * @return A detailed description of the widget's functions
     */
    QString whatsThis() const;
    /**
     * Creates a new instance of this widget to show it on the designer
     * @param parent This param specifies the parent for the widget which is being created.
     * @return A new instance of this widget
     */
    QWidget *createWidget(QWidget *parent);
    /**
     * Initializes a new instance of this wrapper if hasn't been donde previously
     */
    void initialize(QDesignerFormEditorInterface *core);

 private:
     bool initialized;
 };

#endif // QT5WAYBUTTONPLUGIN_H
