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

#ifndef EWIFRAMEWORKWIDGETCOLLECTION_H
#define EWIFRAMEWORKWIDGETCOLLECTION_H

#include <QObject>
#include <QtDesigner/QtDesigner>

/**
 * This class exposes the entire collection of Embedded Widgets made by Nokia.
 *
 * To expose the entire widget collection to the designer, this class acts as a holder of a collection
 * of widgets to be exposed to the designer. It holds all of them in a list accessed by the designer.
 *
 * This class is an implementation of the QDesignerCustomWidgetCollectionInterface interface.
 *
 * @see QDesignerCustomWidgetCollectionInterface.
 * @author Roberto Estrada Casarrubios
 */
class EWIFrameworkWidgetCollection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
 public:
     /**
      * Class constructor. Sets up the widget collection to be shown to the designer.
      *
      * If a new Widget is added to the framework, it must be exposed to the designer by adding
      * to the custom widget list here.
      *
      * @param parent The parent of all the widgets of the collection, it can be NULL.
      */
     EWIFrameworkWidgetCollection(QObject *parent = 0);
     /**
      * Returns to anyone who requests it the list of the framework widgets which are exposed
      * to the designer.
      */
     virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;
 private:
     QList<QDesignerCustomWidgetInterface*> widgets;
};

#endif // EWIFRAMEWORKWIDGETCOLLECTION_H
