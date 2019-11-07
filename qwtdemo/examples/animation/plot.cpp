#include <qapplication.h>
#include <qwt_math.h>
#include <qwt_symbol.h>
#include <qwt_curve_fitter.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qevent.h>
#include "plot.h"

class Curve: public QwtPlotCurve
{
public:
    void setTransformation( const QTransform &transform )
    {
        d_transform = transform;
    }

    virtual void updateSamples( double phase )
    {
        setSamples( d_transform.map( points( phase ) ) );
    }

private:
    virtual QPolygonF points( double phase ) const = 0;

private:
    QTransform d_transform;
};

class Curve1: public Curve
{
public:
    Curve1()
    {
        setPen( QColor( 150, 150, 200 ), 2 );
        setStyle( QwtPlotCurve::Lines );

        QwtSplineCurveFitter *curveFitter = new QwtSplineCurveFitter();
        curveFitter->setSplineSize( 150 );
        setCurveFitter( curveFitter );

        setCurveAttribute( QwtPlotCurve::Fitted, true );

        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::XCross );
        symbol->setPen( Qt::yellow );
        symbol->setSize( 7 );

        setSymbol( symbol );

        // somewhere to the left
        QTransform transform;
        transform.scale( 1.5, 1.0 );
        transform.translate( 1.5, 3.0 );

        setTransformation( transform );
    }

    virtual QPolygonF points( double phase ) const
    {
        QPolygonF points;

        const int numSamples = 15;
        for ( int i = 0; i < numSamples; i++ )
        {
            const double v = 6.28 * double( i ) / double( numSamples - 1 );
            points += QPointF( qSin( v - phase ), v );
        }

        return points;
    }
};

class Curve2: public Curve
{
public:
    Curve2()
    {
        setStyle( QwtPlotCurve::Sticks );
        setPen( QColor( 200, 150, 50 ) );

        setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
            QColor( Qt::gray ), QColor( Qt::yellow ), QSize( 5, 5 ) ) );
    }

private:
    virtual QPolygonF points( double phase ) const
    {
        QPolygonF points;

        const int numSamples = 50;
        for ( int i = 0; i < numSamples; i++ )
        {
            const double v = 10.0 * i / double( numSamples - 1 );
            points += QPointF( v, qCos( 3.0 * ( v + phase ) ) );
        }

        return points;
    }
};

class Curve3: public Curve
{       
public: 
    Curve3()
    {
        setStyle( QwtPlotCurve::Lines );
        setPen( QColor( 100, 200, 150 ), 2 );

        QwtSplineCurveFitter* curveFitter = new QwtSplineCurveFitter();
        curveFitter->setFitMode( QwtSplineCurveFitter::ParametricSpline );
        curveFitter->setSplineSize( 200 );
        setCurveFitter( curveFitter );

        setCurveAttribute( QwtPlotCurve::Fitted, true );

        // somewhere in the top right corner
        QTransform transform;
        transform.translate( 7.0, 7.5 );
        transform.scale( 2.0, 2.0 );

        setTransformation( transform );
    }   

private:
    virtual QPolygonF points( double phase ) const
    {
        QPolygonF points;

        const int numSamples = 9;
        for ( int i = 0; i < numSamples; i++ )
        {
            const double v = i * 2.0 * M_PI / ( numSamples - 1 );
            points += QPointF( qSin( v - phase ), qCos( 3.0 * ( v + phase ) ) );
        }

        return points;
    }
};  

class Curve4: public Curve
{       
public: 
    Curve4()
    {
        setStyle( QwtPlotCurve::Lines );
        setPen( Qt::red, 2 );

        initSamples();

        // somewhere in the center
        QTransform transform;
        transform.translate( 7.0, 3.0 );
        transform.scale( 1.5, 1.5 );

        setTransformation( transform );
    }   

private:
    virtual QPolygonF points( double phase ) const
    {
        const double speed = 0.05;

        const double s = speed * qSin( phase );
        const double c = qSqrt( 1.0 - s * s );

        for ( int i = 0; i < d_points.size(); i++ )
        {
            const QPointF p = d_points[i];

            const double u = p.x();
            const double v = p.y();

            d_points[i].setX( u * c - v * s );
            d_points[i].setY( v * c + u * s );
        }

        return d_points;
    }

    void initSamples()
    {
        const int numSamples = 15;

        for ( int i = 0; i < numSamples; i++ )
        {
            const double angle = i * ( 2.0 * M_PI / ( numSamples - 1 ) );

            QPointF p( qCos( angle ), qSin( angle ) );
            if ( i % 2 )
                p *= 0.4;
            
            d_points += p;
        }
    }

private:
    mutable QPolygonF d_points;
};  

Plot::Plot( QWidget *parent ):
    QwtPlot( parent)
{
    setAutoReplot( false );

    setTitle( "Animated Curves" );

    // hide all axes
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        enableAxis( axis, false );

    plotLayout()->setCanvasMargin( 10 );

    d_curves[0] = new Curve1();
    d_curves[1] = new Curve2();
    d_curves[2] = new Curve3();
    d_curves[3] = new Curve4();

    updateCurves();

    for ( int i = 0; i < CurveCount; i++ )
        d_curves[i]->attach( this );

    d_time.start();
    ( void )startTimer( 40 );
}

void Plot::timerEvent( QTimerEvent * )
{
    updateCurves();
    replot();
}

void Plot::updateCurves()
{
    const double speed = 2 * M_PI / 25000.0; // a cycle every 25 seconds

    const double phase = d_time.elapsed() * speed;
    for ( int i = 0; i < CurveCount; i++ )
        d_curves[i]->updateSamples( phase );
}
