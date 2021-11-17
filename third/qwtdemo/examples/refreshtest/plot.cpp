#include <qglobal.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#ifndef QWT_NO_OPENGL
#include <qevent.h>
#include <qwt_plot_glcanvas.h>
#endif
#include "plot.h"
#include "circularbuffer.h"
#include "settings.h"

static double wave( double x )
{
    const double period = 1.0;
    const double c = 5.0;

    double v = ::fmod( x, period );

    const double amplitude = qAbs( x - qRound( x / c ) * c ) / ( 0.5 * c );
    v = amplitude * qSin( v / period * 2 * M_PI );

    return v;
}

static double noise( double )
{
    return 2.0 * ( qrand() / ( static_cast<double>( RAND_MAX ) + 1 ) ) - 1.0;
}

#ifndef QWT_NO_OPENGL
class GLCanvas: public QwtPlotGLCanvas
{
public:
    GLCanvas( QwtPlot *parent = NULL ):
        QwtPlotGLCanvas( parent )
    {
        setContentsMargins( 1, 1, 1, 1 );
    }

protected:
    virtual void paintEvent( QPaintEvent *event )
    {
        QPainter painter( this );
        painter.setClipRegion( event->region() );

        QwtPlot *plot = qobject_cast< QwtPlot *>( parent() );
        if ( plot )
            plot->drawCanvas( &painter );

        painter.setPen( palette().foreground().color() );
#if QT_VERSION >= 0x050000
        painter.drawRect( rect().adjusted( 1, 1, 0, 0 ) );
#else
        painter.drawRect( rect().adjusted( 0, 0, -1, -1 ) );
#endif
    }
};
#endif

Plot::Plot( QWidget *parent ):
    QwtPlot( parent ),
    d_interval( 10.0 ), // seconds
    d_timerId( -1 )
{
    // Assign a title
    setTitle( "Testing Refresh Rates" );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setLineWidth( 1 );
    canvas->setPalette( Qt::white );

    setCanvas( canvas );

    alignScales();

    // Insert grid
    d_grid = new QwtPlotGrid();
    d_grid->attach( this );

    // Insert curve
    d_curve = new QwtPlotCurve( "Data Moving Right" );
    d_curve->setPen( Qt::black );
    d_curve->setData( new CircularBuffer( d_interval, 10 ) );
    d_curve->attach( this );

    // Axis
    setAxisTitle( QwtPlot::xBottom, "Seconds" );
    setAxisScale( QwtPlot::xBottom, -d_interval, 0.0 );

    setAxisTitle( QwtPlot::yLeft, "Values" );
    setAxisScale( QwtPlot::yLeft, -1.0, 1.0 );

    d_clock.start();

    setSettings( d_settings );
}

//
//  Set a plain canvas frame and align the scales to it
//
void Plot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = axisWidget( i );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );

        QwtScaleDraw *scaleDraw = axisScaleDraw( i );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }

    plotLayout()->setAlignCanvasToScales( true );
}

void Plot::setSettings( const Settings &s )
{
    if ( d_timerId >= 0 )
        killTimer( d_timerId );

    d_timerId = startTimer( s.updateInterval );

    d_grid->setPen( s.grid.pen );
    d_grid->setVisible( s.grid.pen.style() != Qt::NoPen );

    CircularBuffer *buffer = static_cast<CircularBuffer *>( d_curve->data() );
    if ( s.curve.numPoints != buffer->size() ||
            s.curve.functionType != d_settings.curve.functionType )
    {
        switch( s.curve.functionType )
        {
            case Settings::Wave:
                buffer->setFunction( wave );
                break;
            case Settings::Noise:
                buffer->setFunction( noise );
                break;
            default:
                buffer->setFunction( NULL );
        }

        buffer->fill( d_interval, s.curve.numPoints );
    }

    d_curve->setPen( s.curve.pen );
    d_curve->setBrush( s.curve.brush );

    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons,
        s.curve.paintAttributes & QwtPlotCurve::ClipPolygons );
    d_curve->setPaintAttribute( QwtPlotCurve::FilterPoints,
        s.curve.paintAttributes & QwtPlotCurve::FilterPoints );

    d_curve->setRenderHint( QwtPlotItem::RenderAntialiased,
        s.curve.renderHint & QwtPlotItem::RenderAntialiased );

#ifndef QWT_NO_OPENGL
    if ( s.canvas.openGL )
    {
        QwtPlotGLCanvas *plotCanvas = qobject_cast<QwtPlotGLCanvas *>( canvas() );
        if ( plotCanvas == NULL )
        {
            plotCanvas = new GLCanvas();
            plotCanvas->setPalette( QColor( "khaki" ) );

            setCanvas( plotCanvas );
        }
    }
    else
#endif
    {
        QwtPlotCanvas *plotCanvas = qobject_cast<QwtPlotCanvas *>( canvas() );
        if ( plotCanvas == NULL )
        {
            plotCanvas = new QwtPlotCanvas();
            plotCanvas->setFrameStyle( QFrame::Box | QFrame::Plain );
            plotCanvas->setLineWidth( 1 );
            plotCanvas->setPalette( Qt::white );

            setCanvas( plotCanvas );
        }

        plotCanvas->setAttribute( Qt::WA_PaintOnScreen, s.canvas.paintOnScreen );

        plotCanvas->setPaintAttribute(
            QwtPlotCanvas::BackingStore, s.canvas.useBackingStore );
        plotCanvas->setPaintAttribute(
            QwtPlotCanvas::ImmediatePaint, s.canvas.immediatePaint );
    }

    QwtPainter::setPolylineSplitting( s.curve.lineSplitting );

    d_settings = s;
}

void Plot::timerEvent( QTimerEvent * )
{
    CircularBuffer *buffer = static_cast<CircularBuffer *>( d_curve->data() );
    buffer->setReferenceTime( d_clock.elapsed() / 1000.0 );

    if ( d_settings.updateType == Settings::RepaintCanvas )
    {
        // the axes in this example doesn't change. So all we need to do
        // is to repaint the canvas.

        QMetaObject::invokeMethod( canvas(), "replot", Qt::DirectConnection );
    }
    else
    {
        replot();
    }
}
