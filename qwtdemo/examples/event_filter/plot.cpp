#include "plot.h"
#include "colorbar.h"
#include <qevent.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_widget.h>
#include <qwt_wheel.h>
#include <stdlib.h>

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    setTitle( "Interactive Plot" );

    setCanvasColor( Qt::darkCyan );

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->attach( this );

    // axes

    setAxisScale( QwtPlot::xBottom, 0.0, 100.0 );
    setAxisScale( QwtPlot::yLeft, 0.0, 100.0 );

    // Avoid jumping when label with 3 digits
    // appear/disappear when scrolling vertically

    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( sd->extent( axisWidget( QwtPlot::yLeft )->font() ) );

    plotLayout()->setAlignCanvasToScales( true );

    insertCurve( Qt::Vertical, Qt::blue, 30.0 );
    insertCurve( Qt::Vertical, Qt::magenta, 70.0 );
    insertCurve( Qt::Horizontal, Qt::yellow, 30.0 );
    insertCurve( Qt::Horizontal, Qt::white, 70.0 );

    replot();

    // ------------------------------------
    // We add a color bar to the left axis
    // ------------------------------------

    QwtScaleWidget *scaleWidget = axisWidget( yLeft );
    scaleWidget->setMargin( 10 ); // area for the color bar
    d_colorBar = new ColorBar( Qt::Vertical, scaleWidget );
    d_colorBar->setRange( Qt::red, Qt::darkBlue );
    d_colorBar->setFocusPolicy( Qt::TabFocus );

    connect( d_colorBar, SIGNAL( selected( const QColor & ) ),
        SLOT( setCanvasColor( const QColor & ) ) );

    // we need the resize events, to lay out the color bar
    scaleWidget->installEventFilter( this );

    // ------------------------------------
    // We add a wheel to the canvas
    // ------------------------------------

    d_wheel = new QwtWheel( canvas() );
    d_wheel->setOrientation( Qt::Vertical );
    d_wheel->setRange( -100, 100 );
    d_wheel->setValue( 0.0 );
    d_wheel->setMass( 0.2 );
    d_wheel->setTotalAngle( 4 * 360.0 );

    connect( d_wheel, SIGNAL( valueChanged( double ) ),
        SLOT( scrollLeftAxis( double ) ) );

    // we need the resize events, to lay out the wheel
    canvas()->installEventFilter( this );

    d_colorBar->setWhatsThis(
        "Selecting a color will change the background of the plot." );
    scaleWidget->setWhatsThis(
        "Selecting a value at the scale will insert a new curve." );
    d_wheel->setWhatsThis(
        "With the wheel you can move the visible area." );
    axisWidget( xBottom )->setWhatsThis(
        "Selecting a value at the scale will insert a new curve." );
}

void Plot::setCanvasColor( const QColor &c )
{
    setCanvasBackground( c );
    replot();
}

void Plot::scrollLeftAxis( double value )
{
    setAxisScale( yLeft, value, value + 100.0 );
    replot();
}

bool Plot::eventFilter( QObject *object, QEvent *e )
{
    if ( e->type() == QEvent::Resize )
    {
        const QSize size = static_cast<QResizeEvent *>( e )->size();
        if ( object == axisWidget( yLeft ) )
        {
            const QwtScaleWidget *scaleWidget = axisWidget( yLeft );

            const int margin = 2;

            // adjust the color bar to the scale backbone
            const int x = size.width() - scaleWidget->margin() + margin;
            const int w = scaleWidget->margin() - 2 * margin;
            const int y = scaleWidget->startBorderDist();
            const int h = size.height() -
                scaleWidget->startBorderDist() - scaleWidget->endBorderDist();

            d_colorBar->setGeometry( x, y, w, h );
        }
        if ( object == canvas() )
        {
            const int w = 16;
            const int h = 50;
            const int margin = 2;

            const QRect cr = canvas()->contentsRect();
            d_wheel->setGeometry(
                cr.right() - margin - w, cr.center().y() - h / 2, w, h );
        }
    }

    return QwtPlot::eventFilter( object, e );
}

void Plot::insertCurve( int axis, double base )
{
    Qt::Orientation o;
    if ( axis == yLeft || axis == yRight )
        o = Qt::Horizontal;
    else
        o = Qt::Vertical;

    QRgb rgb = static_cast<QRgb>( rand() );
    insertCurve( o, QColor( rgb ), base );
    replot();
}

void Plot::insertCurve( Qt::Orientation o,
    const QColor &c, double base )
{
    QwtPlotCurve *curve = new QwtPlotCurve();

    curve->setPen( c );
    curve->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
        Qt::gray, c, QSize( 8, 8 ) ) );

    double x[10];
    double y[sizeof( x ) / sizeof( x[0] )];

    for ( uint i = 0; i < sizeof( x ) / sizeof( x[0] ); i++ )
    {
        double v = 5.0 + i * 10.0;
        if ( o == Qt::Horizontal )
        {
            x[i] = v;
            y[i] = base;
        }
        else
        {
            x[i] = base;
            y[i] = v;
        }
    }

    curve->setSamples( x, y, sizeof( x ) / sizeof( x[0] ) );
    curve->attach( this );
}
