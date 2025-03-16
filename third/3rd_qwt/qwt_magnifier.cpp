/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_magnifier.h"
#include "qwt_math.h"
#include <qevent.h>
#include <qwidget.h>

class QwtMagnifier::PrivateData
{
public:
    PrivateData():
        isEnabled( false ),
        wheelFactor( 0.9 ),
        wheelModifiers( Qt::NoModifier ),
        mouseFactor( 0.95 ),
        mouseButton( Qt::RightButton ),
        mouseButtonModifiers( Qt::NoModifier ),
        keyFactor( 0.9 ),
        zoomInKey( Qt::Key_Plus ),
        zoomInKeyModifiers( Qt::NoModifier ),
        zoomOutKey( Qt::Key_Minus ),
        zoomOutKeyModifiers( Qt::NoModifier ),
        mousePressed( false )
    {
    }

    bool isEnabled;

    double wheelFactor;
    Qt::KeyboardModifiers wheelModifiers;

    double mouseFactor;

    Qt::MouseButton mouseButton;
    Qt::KeyboardModifiers mouseButtonModifiers;

    double keyFactor;

    int zoomInKey;
    Qt::KeyboardModifiers zoomInKeyModifiers;

    int zoomOutKey;
    Qt::KeyboardModifiers  zoomOutKeyModifiers;

    bool mousePressed;
    bool hasMouseTracking;
    QPoint mousePos;
};

/*!
   Constructor
   \param parent Widget to be magnified
*/
QwtMagnifier::QwtMagnifier( QWidget *parent ):
    QObject( parent )
{
    d_data = new PrivateData();
    setEnabled( true );
}

//! Destructor
QwtMagnifier::~QwtMagnifier()
{
    delete d_data;
}

/*!
  \brief En/disable the magnifier

  When enabled is true an event filter is installed for
  the observed widget, otherwise the event filter is removed.

  \param on true or false
  \sa isEnabled(), eventFilter()
*/
void QwtMagnifier::setEnabled( bool on )
{
    if ( d_data->isEnabled != on )
    {
        d_data->isEnabled = on;

        QObject *o = parent();
        if ( o )
        {
            if ( d_data->isEnabled )
                o->installEventFilter( this );
            else
                o->removeEventFilter( this );
        }
    }
}

/*!
  \return true when enabled, false otherwise
  \sa setEnabled(), eventFilter()
*/
bool QwtMagnifier::isEnabled() const
{
    return d_data->isEnabled;
}

/*!
   \brief Change the wheel factor

   The wheel factor defines the ratio between the current range
   on the parent widget and the zoomed range for each step of the wheel.

   Use values > 1 for magnification (i.e. 2.0) and values < 1 for
   scaling down (i.e. 1/2.0 = 0.5). You can use this feature for
   inverting the direction of the wheel.

   The default value is 0.9.

   \param factor Wheel factor
   \sa wheelFactor(), setWheelButtonState(),
       setMouseFactor(), setKeyFactor()
*/
void QwtMagnifier::setWheelFactor( double factor )
{
    d_data->wheelFactor = factor;
}

/*!
   \return Wheel factor
   \sa setWheelFactor()
*/
double QwtMagnifier::wheelFactor() const
{
    return d_data->wheelFactor;
}

/*!
   Assign keyboard modifiers for zooming in/out using the wheel.
   The default modifiers are Qt::NoModifiers.

   \param modifiers Keyboard modifiers
   \sa wheelModifiers()
*/
void QwtMagnifier::setWheelModifiers( Qt::KeyboardModifiers modifiers )
{
    d_data->wheelModifiers = modifiers;
}

/*!
   \return Wheel modifiers
   \sa setWheelModifiers()
*/
Qt::KeyboardModifiers QwtMagnifier::wheelModifiers() const
{
    return d_data->wheelModifiers;
}

/*!
   \brief Change the mouse factor

   The mouse factor defines the ratio between the current range
   on the parent widget and the zoomed range for each vertical mouse movement.
   The default value is 0.95.

   \param factor Wheel factor
   \sa mouseFactor(), setMouseButton(), setWheelFactor(), setKeyFactor()
*/
void QwtMagnifier::setMouseFactor( double factor )
{
    d_data->mouseFactor = factor;
}

/*!
   \return Mouse factor
   \sa setMouseFactor()
*/
double QwtMagnifier::mouseFactor() const
{
    return d_data->mouseFactor;
}

/*!
   Assign the mouse button, that is used for zooming in/out.
   The default value is Qt::RightButton.

   \param button Button
   \param modifiers Keyboard modifiers

   \sa getMouseButton()
*/
void QwtMagnifier::setMouseButton(
    Qt::MouseButton button, Qt::KeyboardModifiers modifiers )
{
    d_data->mouseButton = button;
    d_data->mouseButtonModifiers = modifiers;
}

//! \sa setMouseButton()
void QwtMagnifier::getMouseButton(
    Qt::MouseButton &button, Qt::KeyboardModifiers &modifiers ) const
{
    button = d_data->mouseButton;
    modifiers = d_data->mouseButtonModifiers;
}

/*!
   \brief Change the key factor

   The key factor defines the ratio between the current range
   on the parent widget and the zoomed range for each key press of
   the zoom in/out keys. The default value is 0.9.

   \param factor Key factor
   \sa keyFactor(), setZoomInKey(), setZoomOutKey(),
       setWheelFactor, setMouseFactor()
*/
void QwtMagnifier::setKeyFactor( double factor )
{
    d_data->keyFactor = factor;
}

/*!
   \return Key factor
   \sa setKeyFactor()
*/
double QwtMagnifier::keyFactor() const
{
    return d_data->keyFactor;
}

/*!
   Assign the key, that is used for zooming in.
   The default combination is Qt::Key_Plus + Qt::NoModifier.

   \param key
   \param modifiers
   \sa getZoomInKey(), setZoomOutKey()
*/
void QwtMagnifier::setZoomInKey( int key,
    Qt::KeyboardModifiers modifiers )
{
    d_data->zoomInKey = key;
    d_data->zoomInKeyModifiers = modifiers;
}

/*!
   \brief Retrieve the settings of the zoom in key

   \param key Key code, see Qt::Key
   \param modifiers Keyboard modifiers

   \sa setZoomInKey()
*/
void QwtMagnifier::getZoomInKey( int &key,
    Qt::KeyboardModifiers &modifiers ) const
{
    key = d_data->zoomInKey;
    modifiers = d_data->zoomInKeyModifiers;
}

/*!
   Assign the key, that is used for zooming out.
   The default combination is Qt::Key_Minus + Qt::NoModifier.

   \param key
   \param modifiers
   \sa getZoomOutKey(), setZoomOutKey()
*/
void QwtMagnifier::setZoomOutKey( int key,
    Qt::KeyboardModifiers modifiers )
{
    d_data->zoomOutKey = key;
    d_data->zoomOutKeyModifiers = modifiers;
}

/*!
   \brief Retrieve the settings of the zoom out key

   \param key Key code, see Qt::Key
   \param modifiers Keyboard modifiers

   \sa setZoomOutKey()
*/
void QwtMagnifier::getZoomOutKey( int &key,
    Qt::KeyboardModifiers &modifiers ) const
{
    key = d_data->zoomOutKey;
    modifiers = d_data->zoomOutKeyModifiers;
}

/*!
  \brief Event filter

  When isEnabled() is true, the mouse events of the
  observed widget are filtered.

  \param object Object to be filtered
  \param event Event

  \return Forwarded to QObject::eventFilter()

  \sa widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseMoveEvent(), widgetWheelEvent(), widgetKeyPressEvent()
      widgetKeyReleaseEvent()
*/
bool QwtMagnifier::eventFilter( QObject *object, QEvent *event )
{
    if ( object && object == parent() )
    {
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                widgetMousePressEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseMove:
            {
                widgetMouseMoveEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseButtonRelease:
            {
                widgetMouseReleaseEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::Wheel:
            {
                widgetWheelEvent( static_cast<QWheelEvent *>( event ) );
                break;
            }
            case QEvent::KeyPress:
            {
                widgetKeyPressEvent( static_cast<QKeyEvent *>( event ) );
                break;
            }
            case QEvent::KeyRelease:
            {
                widgetKeyReleaseEvent( static_cast<QKeyEvent *>( event ) );
                break;
            }
            default:;
        }
    }
    return QObject::eventFilter( object, event );
}

/*!
  Handle a mouse press event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMouseReleaseEvent(), widgetMouseMoveEvent()
*/
void QwtMagnifier::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
    if ( parentWidget() == NULL )
        return;

    if ( ( mouseEvent->button() != d_data->mouseButton ) ||
        ( mouseEvent->modifiers() != d_data->mouseButtonModifiers ) )
    {
        return;
    }

    d_data->hasMouseTracking = parentWidget()->hasMouseTracking();

    parentWidget()->setMouseTracking( true );
    d_data->mousePos = mouseEvent->pos();
    d_data->mousePressed = true;
}

/*!
  Handle a mouse release event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseMoveEvent(),
*/
void QwtMagnifier::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    if ( d_data->mousePressed && parentWidget() )
    {
        d_data->mousePressed = false;
        parentWidget()->setMouseTracking( d_data->hasMouseTracking );
    }
}

/*!
  Handle a mouse move event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
*/
void QwtMagnifier::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
    if ( !d_data->mousePressed )
        return;

    const int dy = mouseEvent->pos().y() - d_data->mousePos.y();
    if ( dy != 0 )
    {
        double f = d_data->mouseFactor;
        if ( dy < 0 )
            f = 1 / f;

        rescale( f );
    }

    d_data->mousePos = mouseEvent->pos();
}

/*!
  Handle a wheel event for the observed widget.

  \param wheelEvent Wheel event
  \sa eventFilter()
*/
void QwtMagnifier::widgetWheelEvent( QWheelEvent *wheelEvent )
{
    if ( wheelEvent->modifiers() != d_data->wheelModifiers )
    {
        return;
    }

    if ( d_data->wheelFactor != 0.0 )
    {
        /*
            A positive delta indicates that the wheel was
            rotated forwards away from the user; a negative
            value indicates that the wheel was rotated
            backwards toward the user.
            Most mouse types work in steps of 15 degrees,
            in which case the delta value is a multiple
            of 120 (== 15 * 8).
         */
        double f = qPow( d_data->wheelFactor,
            qAbs( wheelEvent->angleDelta().y() / 120.0 ) );

        if ( wheelEvent->angleDelta().y() > 0 )
            f = 1 / f;

        rescale( f );
    }
}

/*!
  Handle a key press event for the observed widget.

  \param keyEvent Key event
  \sa eventFilter(), widgetKeyReleaseEvent()
*/
void QwtMagnifier::widgetKeyPressEvent( QKeyEvent *keyEvent )
{
    if ( keyEvent->key() == d_data->zoomInKey &&
        keyEvent->modifiers() == d_data->zoomInKeyModifiers )
    {
        rescale( d_data->keyFactor );
    }
    else if ( keyEvent->key() == d_data->zoomOutKey &&
        keyEvent->modifiers() == d_data->zoomOutKeyModifiers )
    {
        rescale( 1.0 / d_data->keyFactor );
    }
}

/*!
  Handle a key release event for the observed widget.

  \param keyEvent Key event
  \sa eventFilter(), widgetKeyReleaseEvent()
*/
void QwtMagnifier::widgetKeyReleaseEvent( QKeyEvent *keyEvent )
{
    Q_UNUSED( keyEvent );
}

//! \return Parent widget, where the rescaling happens
QWidget *QwtMagnifier::parentWidget()
{
    return qobject_cast<QWidget *>( parent() );
}

//! \return Parent widget, where the rescaling happens
const QWidget *QwtMagnifier::parentWidget() const
{
    return qobject_cast<const QWidget *>( parent() );
}

