/*
 Embedded Widgets Demo
 Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).*
 Contact:  Qt Software Information (qt-info@nokia.com)**
 This file may be used under the terms of the Embedded Widgets Demo License
 Agreement.
*/

#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>

#include "qtsvgbutton.h"

/*!
    
    \class QtSvgButton qtsvgbutton.h

    The description of the QtSvgButton.

    \brief The QtSvgButton class provides a Button, that can change the display style.

    The QtSvgButton is an example to show the capabilities of the Qt Framework related
    to customized controls.

    Here is a possible implementation shown

    \code

        // Create a QtSvgButton
        QWidget * widget = new QWidget(this)
        QtSvgButton * button = new QtSvgButton(widget);
        butten->setSkin("Beryl");
        widget->addWidget(button);
        widget->show();

    \endcode

    It is important to call the function \a setSkin() to load a skin. If \a setSkin() is not called
    the Button will not have any visible content.
    The parameter \p skin has the skin name. The skins are in the resource file defined in the project file.
    The name scheme in the resource file is "Skin Name"/"unique picture name.svg".

    The \a setSkin() function loads the diffrent skins over the "Skin name".
    The needed realised graphics for this control are "normal.svg", "hovered.svg" and "pressed.svg".
    If you want to add new skins, they have to follow the name scheme to work.

    The actual skin can be read over the \a skin() function.

    \sa skin(), setSkin()

    No Signals are defined.

    No Slots are defined.
*/

/*!
    Constructor of the QtSvgButton.
    The \a parent object holds the Parent of the Widget.
*/
QtSvgButton::QtSvgButton(QWidget * parent)
    : QAbstractButton(parent)
{
    // trigger repaint on enter and leave event
    setAttribute(Qt::WA_Hover, true);
}

/*!
    Destructor
*/
QtSvgButton::~QtSvgButton()
{
}

/*!
    With this function you can set the skin that will be displayed in the widget.

    \code
        QtSvgButton * button = new QtSvgButton(this);
        button->setSkin("Beryl");
    \endcode

    This function has to be called before using the QSvgButton.

    \sa skin()

*/
void QtSvgButton::setSkin(const QString& skin)
{
    m_skin = skin;
    QString base = ":/svgbutton/" + skin + '/';

    // create null size and null pictures
    m_defaultSize = QSize();
    m_normal = QPicture();
    m_hovered = QPicture();
    m_pressed = QPicture();

    QSvgRenderer renderer;
    QPainter painter;

    if (renderer.load(base + "normal.svg")) {
        m_defaultSize = renderer.defaultSize();
        painter.begin(&m_normal);
        renderer.render(&painter, QRectF(0.0, 0.0, 1.0, 1.0));
        painter.end();
    }

    if (renderer.load(base + "pressed.svg")) {
        painter.begin(&m_pressed);
        renderer.render(&painter, QRectF(0.0, 0.0, 1.0, 1.0));
        painter.end();
    }

    if (renderer.load(base + "hovered.svg")) {
        painter.begin(&m_hovered);
        renderer.render(&painter, QRectF(0.0, 0.0, 1.0, 1.0));
        painter.end();
    }

    // update geometry for new sizeHint and repaint
    updateGeometry();
    update();
}

/*!

    This function returns the actual skin name. If no skin has been set the return value is "".

    \code
        QtSvgButton * button = new QtSvgButton(this);
        button->setSkin("Beryl");
        qDebug() << button->skin();
    \endcode

    \sa setSkin()

*/
QString QtSvgButton::skin() const
{
    return m_skin;
}

/*!

    This function calculates the possibe Button size. This is needed to keep the aspect ratio even
    if the space for the widget is bigger.

*/
QRect QtSvgButton::buttonRect() const
{
    // Keep aspect ratio:
    QSize buttonSize = m_defaultSize;
    buttonSize.scale(rect().size(), Qt::KeepAspectRatio);

    // Keep the button aligned horizontally centered (like the text):
    int buttonPosX = (rect().width() - buttonSize.width()) / 2;
    if (buttonPosX < 0) buttonPosX = 0;
    buttonPosX += rect().left();

    QPoint buttonPos = QPoint(buttonPosX, 0);

    return QRect(buttonPos, buttonSize);
}

/*!

    \overload

    Overloaded paint event. In this function is the set skin painted.

    \sa setSkin(), skin()

*/
void QtSvgButton::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.scale(buttonRect().width(), buttonRect().height());

    if (isDown()) {
        m_pressed.play(&painter);
    } else if (underMouse()) {
        m_hovered.play(&painter);
    } else {
        m_normal.play(&painter);
    }
}

/*!
    \overload
    Return size hint provided by the SVG graphics.
    Can be changed during runtime.
*/
QSize QtSvgButton::sizeHint() const
{
    if (!m_defaultSize.isEmpty()) {
        return m_defaultSize;
    } else {
        return QSize(100, 24);
    }
}
