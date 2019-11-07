#include "editor.h"
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_scale_map.h>
#include <qwt_plot_shapeitem.h>
#include <qevent.h>

class Overlay: public QwtWidgetOverlay
{
public:
    Overlay( QWidget *parent, Editor *editor ):
        QwtWidgetOverlay( parent ),
        d_editor( editor )
    {
        switch( editor->mode() )
        {
            case Editor::NoMask:
            {
                setMaskMode( QwtWidgetOverlay::NoMask );
                setRenderMode( QwtWidgetOverlay::AutoRenderMode );
                break;
            }
            case Editor::Mask:
            {
                setMaskMode( QwtWidgetOverlay::MaskHint );
                setRenderMode( QwtWidgetOverlay::AutoRenderMode );
                break;
            }
            case Editor::AlphaMask:
            {
                setMaskMode( QwtWidgetOverlay::AlphaMask );
                setRenderMode( QwtWidgetOverlay::AutoRenderMode );
                break;
            }
            case Editor::AlphaMaskRedraw:
            {
                setMaskMode( QwtWidgetOverlay::AlphaMask );
                setRenderMode( QwtWidgetOverlay::DrawOverlay );
                break;
            }
            case Editor::AlphaMaskCopyMask:
            {
                setMaskMode( QwtWidgetOverlay::AlphaMask );
                setRenderMode( QwtWidgetOverlay::CopyAlphaMask );
                break;
            }
        }
    }

protected:
    virtual void drawOverlay( QPainter *painter ) const
    {
        d_editor->drawOverlay( painter );
    }

    virtual QRegion maskHint() const
    {
        return d_editor->maskHint();
    }

private:
    Editor *d_editor;
};

Editor::Editor( QwtPlot* plot ):
    QObject( plot ),
    d_isEnabled( false ),
    d_overlay( NULL ),
    d_mode( Mask )
{
    setEnabled( true );
}


Editor::~Editor()
{
    delete d_overlay;
}

QwtPlot *Editor::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *Editor::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

void Editor::setMode( Mode mode )
{
    d_mode = mode;
}

Editor::Mode Editor::mode() const
{
    return d_mode;
}

void Editor::setEnabled( bool on )
{
    if ( on == d_isEnabled )
        return;

    QwtPlot *plot = qobject_cast<QwtPlot *>( parent() );
    if ( plot )
    {
        d_isEnabled = on;

        if ( on )
        {
            plot->canvas()->installEventFilter( this );
        }
        else
        {
            plot->canvas()->removeEventFilter( this );

            delete d_overlay;
            d_overlay = NULL;
        }
    }
}

bool Editor::isEnabled() const
{
    return d_isEnabled;
}

bool Editor::eventFilter( QObject* object, QEvent* event )
{
    QwtPlot *plot = qobject_cast<QwtPlot *>( parent() );
    if ( plot && object == plot->canvas() )
    {
        switch( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                const QMouseEvent* mouseEvent =
                    dynamic_cast<QMouseEvent* >( event );

                if ( d_overlay == NULL && 
                    mouseEvent->button() == Qt::LeftButton  )
                {
                    const bool accepted = pressed( mouseEvent->pos() );
                    if ( accepted )
                    {
                        d_overlay = new Overlay( plot->canvas(), this );

                        d_overlay->updateOverlay();
                        d_overlay->show();
                    }
                }

                break;
            }
            case QEvent::MouseMove:
            {
                if ( d_overlay )
                {
                    const QMouseEvent* mouseEvent =
                        dynamic_cast< QMouseEvent* >( event );

                    const bool accepted = moved( mouseEvent->pos() );
                    if ( accepted )
                        d_overlay->updateOverlay();
                }

                break;
            }
            case QEvent::MouseButtonRelease:
            {
                const QMouseEvent* mouseEvent =
                    static_cast<QMouseEvent* >( event );

                if ( d_overlay && mouseEvent->button() == Qt::LeftButton )
                {
                    released( mouseEvent->pos() );

                    delete d_overlay;
                    d_overlay = NULL;
                }

                break;
            }
            default:
                break;
        }

        return false;
    }

    return QObject::eventFilter( object, event );
}

bool Editor::pressed( const QPoint& pos )
{
    d_editedItem = itemAt( pos );
    if ( d_editedItem )
    {
        d_currentPos = pos;
        setItemVisible( d_editedItem, false );

        return true;
    }

    return false; // don't accept the position
}

bool Editor::moved( const QPoint& pos )
{
    if ( plot() == NULL )
        return false;

    const QwtScaleMap xMap = plot()->canvasMap( d_editedItem->xAxis() );
    const QwtScaleMap yMap = plot()->canvasMap( d_editedItem->yAxis() );

    const QPointF p1 = QwtScaleMap::invTransform( xMap, yMap, d_currentPos );
    const QPointF p2 = QwtScaleMap::invTransform( xMap, yMap, pos );

#if QT_VERSION >= 0x040600
    const QPainterPath shape = d_editedItem->shape().translated( p2 - p1 );
#else
    const double dx = p2.x() - p1.x();
    const double dy = p2.y() - p1.y();

    QPainterPath shape = d_editedItem->shape();
    for ( int i = 0; i < shape.elementCount(); i++ )
    {
        const QPainterPath::Element &el = shape.elementAt( i );
        shape.setElementPositionAt( i, el.x + dx, el.y + dy );
    }
#endif

    d_editedItem->setShape( shape );
    d_currentPos = pos;

    return true;
}

void Editor::released( const QPoint& pos )
{
    Q_UNUSED( pos );

    if ( d_editedItem  )
    {
        raiseItem( d_editedItem );
        setItemVisible( d_editedItem, true );
    }
}

QwtPlotShapeItem* Editor::itemAt( const QPoint& pos ) const
{
    const QwtPlot *plot = this->plot();
    if ( plot == NULL )
        return NULL;

    // translate pos into the plot coordinates
    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] =
        plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ] =
        plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ] =
        plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ] =
        plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    QwtPlotItemList items = plot->itemList();
    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        QwtPlotItem *item = items[ i ];
        if ( item->isVisible() &&
            item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *shapeItem = static_cast<QwtPlotShapeItem *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( shapeItem->boundingRect().contains( p )
                && shapeItem->shape().contains( p ) )
            {
                return shapeItem;
            }
        }
    }

    return NULL;
}

QRegion Editor::maskHint() const
{
    return maskHint( d_editedItem );
}

QRegion Editor::maskHint( QwtPlotShapeItem *shapeItem ) const
{
    const QwtPlot *plot = this->plot();
    if ( plot == NULL || shapeItem == NULL )
        return QRegion();

    const QwtScaleMap xMap = plot->canvasMap( shapeItem->xAxis() );
    const QwtScaleMap yMap = plot->canvasMap( shapeItem->yAxis() );

    QRect rect = QwtScaleMap::transform( xMap, yMap,
        shapeItem->shape().boundingRect() ).toRect();

    const int m = 5; // some margin for the pen
    return rect.adjusted( -m, -m, m, m );
}

void Editor::drawOverlay( QPainter* painter ) const
{
    const QwtPlot *plot = this->plot();
    if ( plot == NULL || d_editedItem == NULL )
        return;

    const QwtScaleMap xMap = plot->canvasMap( d_editedItem->xAxis() );
    const QwtScaleMap yMap = plot->canvasMap( d_editedItem->yAxis() );

    painter->setRenderHint( QPainter::Antialiasing,
        d_editedItem->testRenderHint( QwtPlotItem::RenderAntialiased ) );
    d_editedItem->draw( painter, xMap, yMap,
        plot->canvas()->contentsRect() );
}

void Editor::raiseItem( QwtPlotShapeItem *shapeItem )
{
    const QwtPlot *plot = this->plot();
    if ( plot == NULL || shapeItem == NULL )
        return;

    const QwtPlotItemList items = plot->itemList();
    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        QwtPlotItem *item = items[ i ];
        if ( shapeItem == item )
            return;

        if ( item->isVisible() &&
            item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            shapeItem->setZ( item->z() + 1 );
            return;
        }
    }
}

void Editor::setItemVisible( QwtPlotShapeItem *item, bool on )
{
    if ( plot() == NULL || item == NULL || item->isVisible() == on )
        return;

    const bool doAutoReplot = plot()->autoReplot();
    plot()->setAutoReplot( false );

    item->setVisible( on );

    plot()->setAutoReplot( doAutoReplot );

    /*
      Avoid replot with a full repaint of the canvas. 
      For special combinations - f.e. using the 
      raster paint engine on a remote display -
      this makes a difference.
     */

    QwtPlotCanvas *canvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );
    if ( canvas )
        canvas->invalidateBackingStore();

    plot()->canvas()->update( maskHint( item ) );

}

