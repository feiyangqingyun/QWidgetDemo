#include "mainwindow.h"
#include "plot.h"
#include <qmath.h>

static double randomValue()
{
    // a number between [ 0.0, 1.0 ]
    return ( qrand() % 100000 ) / 100000.0;
}

MainWindow::MainWindow()
{
    d_plot = new Plot( this );
    d_plot->setTitle( "Scatter Plot" );
    setCentralWidget( d_plot );

    // a million points
    setSamples( 100000 );
}

void MainWindow::setSamples( int numPoints )
{
    QPolygonF samples;

    for ( int i = 0; i < numPoints; i++ )
    {
        const double x = randomValue() * 24.0 + 1.0;
        const double y = ::log( 10.0 * ( x - 1.0 ) + 1.0 ) 
            * ( randomValue() * 0.5 + 0.9 );

        samples += QPointF( x, y );
    }

    d_plot->setSamples( samples );
}
