/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_painter.h"
#include "qwt_math.h"
#include "qwt_widget_overlay.h"
#include <qapplication.h>
#include <qevent.h>
#include <qpainter.h>
#include <qframe.h>
#include <qcursor.h>
#include <qbitmap.h>
#include <qpointer.h>
#include <qpaintengine.h>
#include <qmath.h>

static inline QRegion qwtMaskRegion( const QRect &r, int penWidth )
{
    const int pw = qMax( penWidth, 1 );
    const int pw2 = penWidth / 2;

    int x1 = r.left() - pw2;
    int x2 = r.right() + 1 + pw2 + ( pw % 2 );

    int y1 = r.top() - pw2;
    int y2 = r.bottom() + 1 + pw2 + ( pw % 2 );

    QRegion region;

    region += QRect( x1, y1, x2 - x1, pw );
    region += QRect( x1, y1, pw, y2 - y1 );
    region += QRect( x1, y2 - pw, x2 - x1, pw );
    region += QRect( x2 - pw, y1, pw, y2 - y1 );

    return region;
}

static inline QRegion qwtMaskRegion( const QLine &l, int penWidth )
{
    const int pw = qMax( penWidth, 1 );
    const int pw2 = penWidth / 2;

    QRegion region;

    if ( l.x1() == l.x2() )
    {
        region += QRect( l.x1() - pw2, l.y1(),
            pw, l.y2() ).normalized();
    }
    else if ( l.y1() == l.y2() )
    {
        region += QRect( l.x1(), l.y1() - pw2,
            l.x2(), pw ).normalized();
    }

    return region;
}

class QwtPickerRubberband: public QwtWidgetOverlay
{
public:
    QwtPickerRubberband( QwtPicker *, QWidget * );

protected:
    virtual void drawOverlay( QPainter * ) const;
    virtual QRegion maskHint() const;

    QwtPicker *d_picker;
};

class QwtPickerTracker: public QwtWidgetOverlay
{
public:
    QwtPickerTracker( QwtPicker *, QWidget * );

protected:
    virtual void drawOverlay( QPainter * ) const;
    virtual QRegion maskHint() const;

    QwtPicker *d_picker;
};


class QwtPicker::PrivateData
{
public:
    PrivateData():
        enabled( false ),
        stateMachine( NULL ),
        resizeMode( QwtPicker::Stretch ),
        rubberBand( QwtPicker::NoRubberBand ),
        trackerMode( QwtPicker::AlwaysOff ),
        isActive( false ),
        trackerPosition( -1, -1 ),
        mouseTracking( false ),
        openGL( false )
    {
    }

    bool enabled;

    QwtPickerMachine *stateMachine;

    QwtPicker::ResizeMode resizeMode;

    QwtPicker::RubberBand rubberBand;
    QPen rubberBandPen;

    QwtPicker::DisplayMode trackerMode;
    QPen trackerPen;
    QFont trackerFont;

    QPolygon pickedPoints;
    bool isActive;
    QPoint trackerPosition;

    bool mouseTracking; // used to save previous value

    QPointer< QwtPickerRubberband > rubberBandOverlay;
    QPointer< QwtPickerTracker> trackerOverlay;

    bool openGL;
};

QwtPickerRubberband::QwtPickerRubberband(
        QwtPicker *picker, QWidget *parent ):
    QwtWidgetOverlay( parent ),
    d_picker( picker )
{
    setMaskMode( QwtWidgetOverlay::MaskHint );
}

QRegion QwtPickerRubberband::maskHint() const
{
    return d_picker->rubberBandMask();
}

void QwtPickerRubberband::drawOverlay( QPainter *painter ) const
{
    painter->setPen( d_picker->rubberBandPen() );
    d_picker->drawRubberBand( painter );
}

QwtPickerTracker::QwtPickerTracker(
        QwtPicker *picker, QWidget *parent ):
    QwtWidgetOverlay( parent ),
    d_picker( picker )
{
    setMaskMode( QwtWidgetOverlay::MaskHint );
}

QRegion QwtPickerTracker::maskHint() const
{
    return d_picker->trackerRect( font() );
}

void QwtPickerTracker::drawOverlay( QPainter *painter ) const
{
    painter->setPen( d_picker->trackerPen() );
    d_picker->drawTracker( painter );
}

/*!
  Constructor

  Creates an picker that is enabled, but without a state machine.
  rubber band and tracker are disabled.

  \param parent Parent widget, that will be observed
 */

QwtPicker::QwtPicker( QWidget *parent ):
    QObject( parent )
{
    init( parent, NoRubberBand, AlwaysOff );
}

/*!
  Constructor

  \param rubberBand Rubber band style
  \param trackerMode Tracker mode
  \param parent Parent widget, that will be observed
 */
QwtPicker::QwtPicker( RubberBand rubberBand,
        DisplayMode trackerMode, QWidget *parent ):
    QObject( parent )
{
    init( parent, rubberBand, trackerMode );
}

//! Destructor
QwtPicker::~QwtPicker()
{
    setMouseTracking( false );

    delete d_data->stateMachine;
    delete d_data->rubberBandOverlay;
    delete d_data->trackerOverlay;

    delete d_data;
}

//! Initialize the picker - used by the constructors
void QwtPicker::init( QWidget *parent,
    RubberBand rubberBand, DisplayMode trackerMode )
{
    d_data = new PrivateData;

    d_data->rubberBand = rubberBand;

    if ( parent )
    {
        if ( parent->focusPolicy() == Qt::NoFocus )
            parent->setFocusPolicy( Qt::WheelFocus );

        d_data->openGL = parent->inherits( "QGLWidget" );
        d_data->trackerFont = parent->font();
        d_data->mouseTracking = parent->hasMouseTracking();

        setEnabled( true );
    }

    setTrackerMode( trackerMode );
}

/*!
  Set a state machine and delete the previous one

  \param stateMachine State machine
  \sa stateMachine()
*/
void QwtPicker::setStateMachine( QwtPickerMachine *stateMachine )
{
    if ( d_data->stateMachine != stateMachine )
    {
        reset();

        delete d_data->stateMachine;
        d_data->stateMachine = stateMachine;

        if ( d_data->stateMachine )
            d_data->stateMachine->reset();
    }
}

/*!
  \return Assigned state machine
  \sa setStateMachine()
*/
QwtPickerMachine *QwtPicker::stateMachine()
{
    return d_data->stateMachine;
}

/*!
  \return Assigned state machine
  \sa setStateMachine()
*/
const QwtPickerMachine *QwtPicker::stateMachine() const
{
    return d_data->stateMachine;
}

//! Return the parent widget, where the selection happens
QWidget *QwtPicker::parentWidget()
{
    QObject *obj = parent();
    if ( obj && obj->isWidgetType() )
        return static_cast<QWidget *>( obj );

    return NULL;
}

//! Return the parent widget, where the selection happens
const QWidget *QwtPicker::parentWidget() const
{
    QObject *obj = parent();
    if ( obj && obj->isWidgetType() )
        return static_cast< const QWidget *>( obj );

    return NULL;
}

/*!
  Set the rubber band style

  \param rubberBand Rubber band style
         The default value is NoRubberBand.

  \sa rubberBand(), RubberBand, setRubberBandPen()
*/
void QwtPicker::setRubberBand( RubberBand rubberBand )
{
    d_data->rubberBand = rubberBand;
}

/*!
  \return Rubber band style
  \sa setRubberBand(), RubberBand, rubberBandPen()
*/
QwtPicker::RubberBand QwtPicker::rubberBand() const
{
    return d_data->rubberBand;
}

/*!
  \brief Set the display mode of the tracker.

  A tracker displays information about current position of
  the cursor as a string. The display mode controls
  if the tracker has to be displayed whenever the observed
  widget has focus and cursor (AlwaysOn), never (AlwaysOff), or
  only when the selection is active (ActiveOnly).

  \param mode Tracker display mode

  \warning In case of AlwaysOn, mouseTracking will be enabled
           for the observed widget.
  \sa trackerMode(), DisplayMode
*/

void QwtPicker::setTrackerMode( DisplayMode mode )
{
    if ( d_data->trackerMode != mode )
    {
        d_data->trackerMode = mode;
        setMouseTracking( d_data->trackerMode == AlwaysOn );
    }
}

/*!
  \return Tracker display mode
  \sa setTrackerMode(), DisplayMode
*/
QwtPicker::DisplayMode QwtPicker::trackerMode() const
{
    return d_data->trackerMode;
}

/*!
  \brief Set the resize mode.

  The resize mode controls what to do with the selected points of an active
  selection when the observed widget is resized.

  Stretch means the points are scaled according to the new
  size, KeepSize means the points remain unchanged.

  The default mode is Stretch.

  \param mode Resize mode
  \sa resizeMode(), ResizeMode
*/
void QwtPicker::setResizeMode( ResizeMode mode )
{
    d_data->resizeMode = mode;
}

/*!
  \return Resize mode
  \sa setResizeMode(), ResizeMode
*/

QwtPicker::ResizeMode QwtPicker::resizeMode() const
{
    return d_data->resizeMode;
}

/*!
  \brief En/disable the picker

  When enabled is true an event filter is installed for
  the observed widget, otherwise the event filter is removed.

  \param enabled true or false
  \sa isEnabled(), eventFilter()
*/
void QwtPicker::setEnabled( bool enabled )
{
    if ( d_data->enabled != enabled )
    {
        d_data->enabled = enabled;

        QWidget *w = parentWidget();
        if ( w )
        {
            if ( enabled )
                w->installEventFilter( this );
            else
                w->removeEventFilter( this );
        }

        updateDisplay();
    }
}

/*!
  \return true when enabled, false otherwise
  \sa setEnabled(), eventFilter()
*/

bool QwtPicker::isEnabled() const
{
    return d_data->enabled;
}

/*!
  Set the font for the tracker

  \param font Tracker font
  \sa trackerFont(), setTrackerMode(), setTrackerPen()
*/
void QwtPicker::setTrackerFont( const QFont &font )
{
    if ( font != d_data->trackerFont )
    {
        d_data->trackerFont = font;
        updateDisplay();
    }
}

/*!
  \return Tracker font
  \sa setTrackerFont(), trackerMode(), trackerPen()
*/

QFont QwtPicker::trackerFont() const
{
    return d_data->trackerFont;
}

/*!
  Set the pen for the tracker

  \param pen Tracker pen
  \sa trackerPen(), setTrackerMode(), setTrackerFont()
*/
void QwtPicker::setTrackerPen( const QPen &pen )
{
    if ( pen != d_data->trackerPen )
    {
        d_data->trackerPen = pen;
        updateDisplay();
    }
}

/*!
  \return Tracker pen
  \sa setTrackerPen(), trackerMode(), trackerFont()
*/
QPen QwtPicker::trackerPen() const
{
    return d_data->trackerPen;
}

/*!
  Set the pen for the rubberband

  \param pen Rubber band pen
  \sa rubberBandPen(), setRubberBand()
*/
void QwtPicker::setRubberBandPen( const QPen &pen )
{
    if ( pen != d_data->rubberBandPen )
    {
        d_data->rubberBandPen = pen;
        updateDisplay();
    }
}

/*!
  \return Rubber band pen
  \sa setRubberBandPen(), rubberBand()
*/
QPen QwtPicker::rubberBandPen() const
{
    return d_data->rubberBandPen;
}

/*!
   \brief Return the label for a position

   In case of HLineRubberBand the label is the value of the
   y position, in case of VLineRubberBand the value of the x position.
   Otherwise the label contains x and y position separated by a ',' .

   The format for the string conversion is "%d".

   \param pos Position
   \return Converted position as string
*/

QwtText QwtPicker::trackerText( const QPoint &pos ) const
{
    QString label;

    switch ( rubberBand() )
    {
        case HLineRubberBand:
            label.asprintf( "%d", pos.y() );
            break;
        case VLineRubberBand:
            label.asprintf( "%d", pos.x() );
            break;
        default:
            label.asprintf( "%d, %d", pos.x(), pos.y() );
    }
    return label;
}

/*!
  Calculate the mask for the rubber band overlay

  \return Region for the mask
  \sa QWidget::setMask()
 */
QRegion QwtPicker::rubberBandMask() const
{
    QRegion mask;

    if ( !isActive() || rubberBand() == NoRubberBand ||
        rubberBandPen().style() == Qt::NoPen )
    {
        return mask;
    }

    const QPolygon pa = adjustedPoints( d_data->pickedPoints );

    QwtPickerMachine::SelectionType selectionType =
        QwtPickerMachine::NoSelection;

    if ( d_data->stateMachine )
        selectionType = d_data->stateMachine->selectionType();

    switch ( selectionType )
    {
        case QwtPickerMachine::NoSelection:
        case QwtPickerMachine::PointSelection:
        {
            if ( pa.count() < 1 )
                return mask;

            const QPoint pos = pa[0];
            const int pw = rubberBandPen().width();

            const QRect pRect = pickArea().boundingRect().toRect();
            switch ( rubberBand() )
            {
                case VLineRubberBand:
                {
                    mask += qwtMaskRegion( QLine( pos.x(), pRect.top(),
                        pos.x(), pRect.bottom() ), pw );
                    break;
                }
                case HLineRubberBand:
                {
                    mask += qwtMaskRegion( QLine( pRect.left(), pos.y(),
                        pRect.right(), pos.y() ), pw );
                    break;
                }
                case CrossRubberBand:
                {
                    mask += qwtMaskRegion( QLine( pos.x(), pRect.top(),
                        pos.x(), pRect.bottom() ), pw );
                    mask += qwtMaskRegion( QLine( pRect.left(), pos.y(),
                        pRect.right(), pos.y() ), pw );
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case QwtPickerMachine::RectSelection:
        {
            if ( pa.count() < 2 )
                return mask;

            const int pw = rubberBandPen().width();

            switch ( rubberBand() )
            {
                case RectRubberBand:
                {
                    const QRect r = QRect( pa.first(), pa.last() );
                    mask = qwtMaskRegion( r.normalized(), pw );
                    break;
                }
                case EllipseRubberBand:
                {
                    const QRect r = QRect( pa.first(), pa.last() );
                    mask += r.adjusted( -pw, -pw, pw, pw );
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case QwtPickerMachine::PolygonSelection:
        {
            const int pw = rubberBandPen().width();
            if ( pw <= 1 )
            {
                // because of the join style we better
                // return a mask for a pen width <= 1 only

                const int off = 2 * pw;
                const QRect r = pa.boundingRect();
                mask += r.adjusted( -off, -off, off, off );
            }
            break;
        }
        default:
            break;
    }

    return mask;
}

/*!
   Draw a rubber band, depending on rubberBand()

   \param painter Painter, initialized with a clip region

   \sa rubberBand(), RubberBand
*/

void QwtPicker::drawRubberBand( QPainter *painter ) const
{
    if ( !isActive() || rubberBand() == NoRubberBand ||
        rubberBandPen().style() == Qt::NoPen )
    {
        return;
    }

    const QPolygon pa = adjustedPoints( d_data->pickedPoints );

    QwtPickerMachine::SelectionType selectionType =
        QwtPickerMachine::NoSelection;

    if ( d_data->stateMachine )
        selectionType = d_data->stateMachine->selectionType();

    switch ( selectionType )
    {
        case QwtPickerMachine::NoSelection:
        case QwtPickerMachine::PointSelection:
        {
            if ( pa.count() < 1 )
                return;

            const QPoint pos = pa[0];

            const QRect pRect = pickArea().boundingRect().toRect();
            switch ( rubberBand() )
            {
                case VLineRubberBand:
                {
                    QwtPainter::drawLine( painter, pos.x(),
                        pRect.top(), pos.x(), pRect.bottom() );
                    break;
                }
                case HLineRubberBand:
                {
                    QwtPainter::drawLine( painter, pRect.left(),
                        pos.y(), pRect.right(), pos.y() );
                    break;
                }
                case CrossRubberBand:
                {
                    QwtPainter::drawLine( painter, pos.x(),
                        pRect.top(), pos.x(), pRect.bottom() );
                    QwtPainter::drawLine( painter, pRect.left(),
                        pos.y(), pRect.right(), pos.y() );
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case QwtPickerMachine::RectSelection:
        {
            if ( pa.count() < 2 )
                return;

            const QRect rect = QRect( pa.first(), pa.last() ).normalized();
            switch ( rubberBand() )
            {
                case EllipseRubberBand:
                {
                    QwtPainter::drawEllipse( painter, rect );
                    break;
                }
                case RectRubberBand:
                {
                    QwtPainter::drawRect( painter, rect );
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case QwtPickerMachine::PolygonSelection:
        {
            if ( rubberBand() == PolygonRubberBand )
                painter->drawPolyline( pa );
            break;
        }
        default:
            break;
    }
}

/*!
   Draw the tracker

   \param painter Painter
   \sa trackerRect(), trackerText()
*/

void QwtPicker::drawTracker( QPainter *painter ) const
{
    const QRect textRect = trackerRect( painter->font() );
    if ( !textRect.isEmpty() )
    {
        const QwtText label = trackerText( d_data->trackerPosition );
        if ( !label.isEmpty() )
            label.draw( painter, textRect );
    }
}

/*!
   \brief Map the pickedPoints() into a selection()

   adjustedPoints() maps the points, that have been collected on
   the parentWidget() into a selection(). The default implementation
   simply returns the points unmodified.

   The reason, why a selection() differs from the picked points
   depends on the application requirements. F.e. :

     - A rectangular selection might need to have a specific aspect ratio only.
     - A selection could accept non intersecting polygons only.
     - ...

   The example below is for a rectangular selection, where the first
   point is the center of the selected rectangle.

  \par Example
  \code
    QPolygon MyPicker::adjustedPoints( const QPolygon &points ) const
    {
        QPolygon adjusted;
        if ( points.size() == 2 )
        {
            const int width = qAbs( points[1].x() - points[0].x() );
            const int height = qAbs( points[1].y() - points[0].y() );

            QRect rect( 0, 0, 2 * width, 2 * height );
            rect.moveCenter( points[0] );

            adjusted += rect.topLeft();
            adjusted += rect.bottomRight();
        }
        return adjusted;
    }
  \endcode
  \endpar

  \param points Selected points
  \return Selected points unmodified
*/
QPolygon QwtPicker::adjustedPoints( const QPolygon &points ) const
{
    return points;
}

/*!
  \return Selected points
  \sa pickedPoints(), adjustedPoints()
*/
QPolygon QwtPicker::selection() const
{
    return adjustedPoints( d_data->pickedPoints );
}

//! \return Current position of the tracker
QPoint QwtPicker::trackerPosition() const
{
    return d_data->trackerPosition;
}

/*!
   Calculate the bounding rectangle for the tracker text
   from the current position of the tracker

   \param font Font of the tracker text
   \return Bounding rectangle of the tracker text

   \sa trackerPosition()
*/
QRect QwtPicker::trackerRect( const QFont &font ) const
{
    if ( trackerMode() == AlwaysOff ||
        ( trackerMode() == ActiveOnly && !isActive() ) )
    {
        return QRect();
    }

    if ( d_data->trackerPosition.x() < 0 || d_data->trackerPosition.y() < 0 )
        return QRect();

    QwtText text = trackerText( d_data->trackerPosition );
    if ( text.isEmpty() )
        return QRect();

    const QSizeF textSize = text.textSize( font );
    QRect textRect( 0, 0, qCeil( textSize.width() ), qCeil( textSize.height() ) );

    const QPoint &pos = d_data->trackerPosition;

    int alignment = 0;
    if ( isActive() && d_data->pickedPoints.count() > 1
        && rubberBand() != NoRubberBand )
    {
        const QPoint last =
            d_data->pickedPoints[ d_data->pickedPoints.count() - 2 ];

        alignment |= ( pos.x() >= last.x() ) ? Qt::AlignRight : Qt::AlignLeft;
        alignment |= ( pos.y() > last.y() ) ? Qt::AlignBottom : Qt::AlignTop;
    }
    else
        alignment = Qt::AlignTop | Qt::AlignRight;

    const int margin = 5;

    int x = pos.x();
    if ( alignment & Qt::AlignLeft )
        x -= textRect.width() + margin;
    else if ( alignment & Qt::AlignRight )
        x += margin;

    int y = pos.y();
    if ( alignment & Qt::AlignBottom )
        y += margin;
    else if ( alignment & Qt::AlignTop )
        y -= textRect.height() + margin;

    textRect.moveTopLeft( QPoint( x, y ) );

    const QRect pickRect = pickArea().boundingRect().toRect();

    int right = qMin( textRect.right(), pickRect.right() - margin );
    int bottom = qMin( textRect.bottom(), pickRect.bottom() - margin );
    textRect.moveBottomRight( QPoint( right, bottom ) );

    int left = qMax( textRect.left(), pickRect.left() + margin );
    int top = qMax( textRect.top(), pickRect.top() + margin );
    textRect.moveTopLeft( QPoint( left, top ) );

    return textRect;
}

/*!
  \brief Event filter

  When isEnabled() is true all events of the observed widget are filtered.
  Mouse and keyboard events are translated into widgetMouse- and widgetKey-
  and widgetWheel-events. Paint and Resize events are handled to keep
  rubber band and tracker up to date.

  \param object Object to be filtered
  \param event Event

  \return Always false.

  \sa widgetEnterEvent(), widgetLeaveEvent(),
      widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent(),
      QObject::installEventFilter(), QObject::event()
*/
bool QwtPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( object && object == parentWidget() )
    {
        switch ( event->type() )
        {
            case QEvent::Resize:
            {
                const QResizeEvent *re = static_cast<QResizeEvent *>( event );

                /*
                   Adding/deleting additional event filters inside of an event filter
                   is not safe dues to the implementation in Qt ( changing alist while iterating ).
                   So we create the overlays in a way, that they don't install en event filter
                   ( parent set to NULL ) and do the resizing here.
                 */
                if ( d_data->trackerOverlay )
                    d_data->trackerOverlay->resize( re->size() );

                if ( d_data->rubberBandOverlay )
                    d_data->rubberBandOverlay->resize( re->size() );

                if ( d_data->resizeMode == Stretch )
                    stretchSelection( re->oldSize(), re->size() );

                updateDisplay();
                break;
            }
            case QEvent::Enter:
            {
                widgetEnterEvent( event );
                break;
            }
            case QEvent::Leave:
            {
                widgetLeaveEvent( event );
                break;
            }
            case QEvent::MouseButtonPress:
            {
                widgetMousePressEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseButtonRelease:
            {
                widgetMouseReleaseEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseButtonDblClick:
            {
                widgetMouseDoubleClickEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseMove:
            {
                widgetMouseMoveEvent( static_cast<QMouseEvent *>( event ) );
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
            case QEvent::Wheel:
            {
                widgetWheelEvent( static_cast<QWheelEvent *>( event ) );
                break;
            }
            default:
                break;
        }
    }
    return false;
}

/*!
  Handle a mouse press event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
    transition( mouseEvent );
}

/*!
  Handle a mouse move event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
    if ( pickArea().contains( mouseEvent->pos() ) )
        d_data->trackerPosition = mouseEvent->pos();
    else
        d_data->trackerPosition = QPoint( -1, -1 );

    if ( !isActive() )
        updateDisplay();

    transition( mouseEvent );
}

/*!
  Handle a enter event for the observed widget.

  \param event Qt event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetEnterEvent( QEvent *event )
{
    transition( event );
}

/*!
  Handle a leave event for the observed widget.

  \param event Qt event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetLeaveEvent( QEvent *event )
{
    transition( event );

    d_data->trackerPosition = QPoint( -1, -1 );
    if ( !isActive() )
        updateDisplay();
}

/*!
  Handle a mouse release event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
    transition( mouseEvent );
}

/*!
  Handle mouse double click event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetMouseDoubleClickEvent( QMouseEvent *mouseEvent )
{
    transition( mouseEvent );
}


/*!
  Handle a wheel event for the observed widget.

  Move the last point of the selection in case of isActive() == true

  \param wheelEvent Wheel event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetKeyPressEvent(), widgetKeyReleaseEvent()
*/
void QwtPicker::widgetWheelEvent( QWheelEvent *wheelEvent )
{
#if (QT_VERSION < QT_VERSION_CHECK(5,15,0))
    if ( pickArea().contains( wheelEvent->pos() ) ) {
        d_data->trackerPosition = wheelEvent->pos();
#else
    if ( pickArea().contains( wheelEvent->position().toPoint() ) ) {
        d_data->trackerPosition = wheelEvent->position().toPoint();
#endif
    } else {
        d_data->trackerPosition = QPoint( -1, -1 );
    }

    updateDisplay();

    transition( wheelEvent );
}

/*!
  Handle a key press event for the observed widget.

  Selections can be completely done by the keyboard. The arrow keys
  move the cursor, the abort key aborts a selection. All other keys
  are handled by the current state machine.

  \param keyEvent Key event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyReleaseEvent(), stateMachine(),
      QwtEventPattern::KeyPatternCode
*/
void QwtPicker::widgetKeyPressEvent( QKeyEvent *keyEvent )
{
    int dx = 0;
    int dy = 0;

    int offset = 1;
    if ( keyEvent->isAutoRepeat() )
        offset = 5;

    if ( keyMatch( KeyLeft, keyEvent ) )
        dx = -offset;
    else if ( keyMatch( KeyRight, keyEvent ) )
        dx = offset;
    else if ( keyMatch( KeyUp, keyEvent ) )
        dy = -offset;
    else if ( keyMatch( KeyDown, keyEvent ) )
        dy = offset;
    else if ( keyMatch( KeyAbort, keyEvent ) )
    {
        reset();
    }
    else
        transition( keyEvent );

    if ( dx != 0 || dy != 0 )
    {
        const QRect rect = pickArea().boundingRect().toRect();
        const QPoint pos = parentWidget()->mapFromGlobal( QCursor::pos() );

        int x = pos.x() + dx;
        x = qMax( rect.left(), x );
        x = qMin( rect.right(), x );

        int y = pos.y() + dy;
        y = qMax( rect.top(), y );
        y = qMin( rect.bottom(), y );

        QCursor::setPos( parentWidget()->mapToGlobal( QPoint( x, y ) ) );
    }
}

/*!
  Handle a key release event for the observed widget.

  Passes the event to the state machine.

  \param keyEvent Key event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseDoubleClickEvent(), widgetMouseMoveEvent(),
      widgetWheelEvent(), widgetKeyPressEvent(), stateMachine()
*/
void QwtPicker::widgetKeyReleaseEvent( QKeyEvent *keyEvent )
{
    transition( keyEvent );
}

/*!
  Passes an event to the state machine and executes the resulting
  commands. Append and Move commands use the current position
  of the cursor ( QCursor::pos() ).

  \param event Event
*/
void QwtPicker::transition( const QEvent *event )
{
    if ( !d_data->stateMachine )
        return;

    const QList<QwtPickerMachine::Command> commandList =
        d_data->stateMachine->transition( *this, event );

    QPoint pos;
    switch ( event->type() )
    {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        {
            const QMouseEvent *me =
                static_cast< const QMouseEvent * >( event );
            pos = me->pos();
            break;
        }
        default:
            pos = parentWidget()->mapFromGlobal( QCursor::pos() );
    }

    for ( int i = 0; i < commandList.count(); i++ )
    {
        switch ( commandList[i] )
        {
            case QwtPickerMachine::Begin:
            {
                begin();
                break;
            }
            case QwtPickerMachine::Append:
            {
                append( pos );
                break;
            }
            case QwtPickerMachine::Move:
            {
                move( pos );
                break;
            }
            case QwtPickerMachine::Remove:
            {
                remove();
                break;
            }
            case QwtPickerMachine::End:
            {
                end();
                break;
            }
        }
    }
}

/*!
  Open a selection setting the state to active

  \sa isActive(), end(), append(), move()
*/
void QwtPicker::begin()
{
    if ( d_data->isActive )
        return;

    d_data->pickedPoints.clear();
    d_data->isActive = true;
    Q_EMIT activated( true );

    if ( trackerMode() != AlwaysOff )
    {
        if ( d_data->trackerPosition.x() < 0 || d_data->trackerPosition.y() < 0 )
        {
            QWidget *w = parentWidget();
            if ( w )
                d_data->trackerPosition = w->mapFromGlobal( QCursor::pos() );
        }
    }

    updateDisplay();
    setMouseTracking( true );
}

/*!
  \brief Close a selection setting the state to inactive.

  The selection is validated and maybe fixed by accept().

  \param ok If true, complete the selection and emit a selected signal
            otherwise discard the selection.
  \return true if the selection is accepted, false otherwise
  \sa isActive(), begin(), append(), move(), selected(), accept()
*/
bool QwtPicker::end( bool ok )
{
    if ( d_data->isActive )
    {
        setMouseTracking( false );

        d_data->isActive = false;
        Q_EMIT activated( false );

        if ( trackerMode() == ActiveOnly )
            d_data->trackerPosition = QPoint( -1, -1 );

        if ( ok )
            ok = accept( d_data->pickedPoints );

        if ( ok )
            Q_EMIT selected( d_data->pickedPoints );
        else
            d_data->pickedPoints.clear();

        updateDisplay();
    }
    else
        ok = false;

    return ok;
}

/*!
   Reset the state machine and terminate ( end(false) ) the selection
*/
void QwtPicker::reset()
{
    if ( d_data->stateMachine )
        d_data->stateMachine->reset();

    if ( isActive() )
        end( false );
}

/*!
  Append a point to the selection and update rubber band and tracker.
  The appended() signal is emitted.

  \param pos Additional point

  \sa isActive(), begin(), end(), move(), appended()
*/
void QwtPicker::append( const QPoint &pos )
{
    if ( d_data->isActive )
    {
        d_data->pickedPoints += pos;

        updateDisplay();
        Q_EMIT appended( pos );
    }
}

/*!
  Move the last point of the selection
  The moved() signal is emitted.

  \param pos New position
  \sa isActive(), begin(), end(), append()
*/
void QwtPicker::move( const QPoint &pos )
{
    if ( d_data->isActive && !d_data->pickedPoints.isEmpty() )
    {
        QPoint &point = d_data->pickedPoints.last();
        if ( point != pos )
        {
            point = pos;

            updateDisplay();
            Q_EMIT moved( pos );
        }
    }
}

/*!
  Remove the last point of the selection
  The removed() signal is emitted.

  \sa isActive(), begin(), end(), append(), move()
*/
void QwtPicker::remove()
{
    if ( d_data->isActive && !d_data->pickedPoints.isEmpty() )
    {
#if QT_VERSION >= 0x050100
        const QPoint pos = d_data->pickedPoints.takeLast();
#else
        const QPoint pos = d_data->pickedPoints.last();
        d_data->pickedPoints.resize( d_data->pickedPoints.count() - 1 );
#endif

        updateDisplay();
        Q_EMIT removed( pos );
    }
}

/*!
  \brief Validate and fix up the selection

  Accepts all selections unmodified

  \param selection Selection to validate and fix up
  \return true, when accepted, false otherwise
*/
bool QwtPicker::accept( QPolygon &selection ) const
{
    Q_UNUSED( selection );
    return true;
}

/*!
  A picker is active between begin() and end().
  \return true if the selection is active.
*/
bool QwtPicker::isActive() const
{
    return d_data->isActive;
}

/*!
  Return the points, that have been collected so far. The selection()
  is calculated from the pickedPoints() in adjustedPoints().
  \return Picked points
*/
const QPolygon &QwtPicker::pickedPoints() const
{
    return d_data->pickedPoints;
}

/*!
  Scale the selection by the ratios of oldSize and newSize
  The changed() signal is emitted.

  \param oldSize Previous size
  \param newSize Current size

  \sa ResizeMode, setResizeMode(), resizeMode()
*/
void QwtPicker::stretchSelection( const QSize &oldSize, const QSize &newSize )
{
    if ( oldSize.isEmpty() )
    {
        // avoid division by zero. But scaling for small sizes also
        // doesn't make much sense, because of rounding losses. TODO ...
        return;
    }

    const double xRatio = double( newSize.width() ) / double( oldSize.width() );
    const double yRatio = double( newSize.height() ) / double( oldSize.height() );

    for ( int i = 0; i < d_data->pickedPoints.count(); i++ )
    {
        QPoint &p = d_data->pickedPoints[i];
        p.setX( qRound( p.x() * xRatio ) );
        p.setY( qRound( p.y() * yRatio ) );

        Q_EMIT changed( d_data->pickedPoints );
    }
}

/*!
  Set mouse tracking for the observed widget.

  In case of enable is true, the previous value
  is saved, that is restored when enable is false.

  \warning Even when enable is false, mouse tracking might be restored
           to true. When mouseTracking for the observed widget
           has been changed directly by QWidget::setMouseTracking
           while mouse tracking has been set to true, this value can't
           be restored.
*/

void QwtPicker::setMouseTracking( bool enable )
{
    QWidget *widget = parentWidget();
    if ( !widget )
        return;

    if ( enable )
    {
        d_data->mouseTracking = widget->hasMouseTracking();
        widget->setMouseTracking( true );
    }
    else
    {
        widget->setMouseTracking( d_data->mouseTracking );
    }
}

/*!
  Find the area of the observed widget, where selection might happen.

  \return parentWidget()->contentsRect()
*/
QPainterPath QwtPicker::pickArea() const
{
    QPainterPath path;

    const QWidget *widget = parentWidget();
    if ( widget )
        path.addRect( widget->contentsRect() );

    return path;
}

//! Update the state of rubber band and tracker label
void QwtPicker::updateDisplay()
{
    QWidget *w = parentWidget();

    bool showRubberband = false;
    bool showTracker = false;

    if ( w && w->isVisible() && d_data->enabled )
    {
        if ( rubberBand() != NoRubberBand && isActive() &&
            rubberBandPen().style() != Qt::NoPen )
        {
            showRubberband = true;
        }

        if ( trackerMode() == AlwaysOn ||
            ( trackerMode() == ActiveOnly && isActive() ) )
        {
            if ( trackerPen() != Qt::NoPen
                && !trackerRect( QFont() ).isEmpty() )
            {
                showTracker = true;
            }
        }
    }

    QPointer< QwtPickerRubberband > &rw = d_data->rubberBandOverlay;
    if ( showRubberband )
    {
        if ( rw.isNull() )
        {
            rw = new QwtPickerRubberband( this, NULL ); // NULL -> no extra event filter
            rw->setObjectName( "PickerRubberBand" );
            rw->setParent( w );
            rw->resize( w->size() );
        }

        if ( d_data->rubberBand <= RectRubberBand )
            rw->setMaskMode( QwtWidgetOverlay::MaskHint );
        else
            rw->setMaskMode( QwtWidgetOverlay::AlphaMask );

        rw->updateOverlay();
    }
    else
    {
        if ( d_data->openGL )
        {
            // Qt 4.8 crashes for a delete
            if ( !rw.isNull() )
            {
                rw->hide();
                rw->deleteLater();
                rw = NULL;
            }
        }
        else
        {
            delete rw;
        }
    }

    QPointer< QwtPickerTracker > &tw = d_data->trackerOverlay;
    if ( showTracker )
    {
        if ( tw.isNull() )
        {
            tw = new QwtPickerTracker( this, NULL ); // NULL -> no extra event filter
            tw->setObjectName( "PickerTracker" );
            tw->setParent( w );
            tw->resize( w->size() );
        }
        tw->setFont( d_data->trackerFont );
        tw->updateOverlay();
    }
    else
    {
        if ( d_data->openGL )
        {
            // Qt 4.8 crashes for a delete
            if ( !tw.isNull() )
            {
                tw->hide();
                tw->deleteLater();
                tw = NULL;
            }
        }
        else
        {
            delete tw;
        }
    }
}

//! \return Overlay displaying the rubber band
const QwtWidgetOverlay *QwtPicker::rubberBandOverlay() const
{
    return d_data->rubberBandOverlay;
}

//! \return Overlay displaying the tracker text
const QwtWidgetOverlay *QwtPicker::trackerOverlay() const
{
    return d_data->trackerOverlay;
}

