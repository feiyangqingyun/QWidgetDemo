#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QwtPlot plot;
    plot.setTitle( "Plot Demo" );
    plot.setCanvasBackground( Qt::white );
    plot.setAxisScale( QwtPlot::yLeft, 0.0, 10.0 );
    plot.insertLegend( new QwtLegend() );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach( &plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 4 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve->setSymbol( symbol );

    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
        << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
        << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 );
    curve->setSamples( points );

    curve->attach( &plot );

    plot.resize( 600, 400 );
    plot.show();

    return a.exec();
}
