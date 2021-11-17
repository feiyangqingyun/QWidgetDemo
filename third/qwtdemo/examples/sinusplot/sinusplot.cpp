#include <qapplication.h>
#include <qlayout.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <math.h>

//-----------------------------------------------------------------
//              simple.cpp
//
//      A simple example which shows how to use QwtPlot connected
//      to a data class without any storage, calculating each values
//      on the fly.
//-----------------------------------------------------------------

class FunctionData: public QwtSyntheticPointData
{
public:
    FunctionData( double( *y )( double ) ):
        QwtSyntheticPointData( 100 ),
        d_y( y )
    {
    }

    virtual double y( double x ) const
    {
        return d_y( x );
    }

private:
    double( *d_y )( double );
};

class ArrowSymbol: public QwtSymbol
{
public:
    ArrowSymbol()
    {
        QPen pen( Qt::black, 0 );
        pen.setJoinStyle( Qt::MiterJoin );

        setPen( pen );
        setBrush( Qt::red );

        QPainterPath path;
        path.moveTo( 0, 8 );
        path.lineTo( 0, 5 );
        path.lineTo( -3, 5 );
        path.lineTo( 0, 0 );
        path.lineTo( 3, 5 );
        path.lineTo( 0, 5 );

        QTransform transform;
        transform.rotate( -30.0 );
        path = transform.map( path );

        setPath( path );
        setPinPoint( QPointF( 0, 0 ) );

        setSize( 10, 14 );
    }
};

class Plot : public QwtPlot
{
public:
    Plot( QWidget *parent = NULL );

protected:
    virtual void resizeEvent( QResizeEvent * );

private:
    void populate();
    void updateGradient();
};


Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    setAutoFillBackground( true );
    setPalette( QPalette( QColor( 165, 193, 228 ) ) );
    updateGradient();

    setTitle( "A Simple QwtPlot Demonstration" );
    insertLegend( new QwtLegend(), QwtPlot::RightLegend );

    // axes
    setAxisTitle( xBottom, "x -->" );
    setAxisScale( xBottom, 0.0, 10.0 );

    setAxisTitle( yLeft, "y -->" );
    setAxisScale( yLeft, -1.0, 1.0 );

    // canvas
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 1 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas->setBorderRadius( 15 );

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    canvas->setPalette( canvasPalette );

    setCanvas( canvas );

    // panning with the left mouse button
    ( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    ( void ) new QwtPlotMagnifier( canvas );

    populate();
}

void Plot::populate()
{
    // Insert new curves
    QwtPlotCurve *cSin = new QwtPlotCurve( "y = sin(x)" );
    cSin->setRenderHint( QwtPlotItem::RenderAntialiased );
    cSin->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );
    cSin->setPen( Qt::red );
    cSin->attach( this );

    QwtPlotCurve *cCos = new QwtPlotCurve( "y = cos(x)" );
    cCos->setRenderHint( QwtPlotItem::RenderAntialiased );
    cCos->setLegendAttribute( QwtPlotCurve::LegendShowLine, true );
    cCos->setPen( Qt::blue );
    cCos->attach( this );

    // Create sin and cos data
    cSin->setData( new FunctionData( ::sin ) );
    cCos->setData( new FunctionData( ::cos ) );

    // Insert markers

    //  ...a horizontal line at y = 0...
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabel( QString::fromLatin1( "y = 0" ) );
    mY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setYValue( 0.0 );
    mY->attach( this );

    //  ...a vertical line at x = 2 * pi
    QwtPlotMarker *mX = new QwtPlotMarker();
    mX->setLabel( QString::fromLatin1( "x = 2 pi" ) );
    mX->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mX->setLabelOrientation( Qt::Vertical );
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( Qt::black, 0, Qt::DashDotLine );
    mX->setXValue( 2.0 * M_PI );
    mX->attach( this );

    const double x = 7.7;

    // an arrow at a specific position
    QwtPlotMarker *mPos = new QwtPlotMarker( "Marker" );
    mPos->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    mPos->setItemAttribute( QwtPlotItem::Legend, true );
    mPos->setSymbol( new ArrowSymbol() );
    mPos->setValue( QPointF( x, ::sin( x ) ) );
    mPos->setLabel( QString( "x = %1" ).arg( x ) );
    mPos->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    mPos->attach( this );
}

void Plot::updateGradient()
{
    QPalette pal = palette();

    const QColor buttonColor = pal.color( QPalette::Button );

    QLinearGradient gradient( rect().topLeft(), rect().bottomLeft() );
    gradient.setColorAt( 0.0, Qt::white );
    gradient.setColorAt( 0.7, buttonColor );
    gradient.setColorAt( 1.0, buttonColor );

    pal.setBrush( QPalette::Window, gradient );
    setPalette( pal );
}

void Plot::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );

    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11
    updateGradient();
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    Plot *plot = new Plot();

    // We put a dummy widget around to have
    // so that Qt paints a widget background
    // when resizing

    QWidget window;
    QHBoxLayout *layout = new QHBoxLayout( &window );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( plot );

    window.resize( 600, 400 );
    window.show();

    return a.exec();
}
