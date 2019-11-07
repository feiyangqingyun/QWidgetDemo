#include "barchart.h"
#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_barchart.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

class DistroScaleDraw: public QwtScaleDraw
{
public:
    DistroScaleDraw( Qt::Orientation orientation, const QStringList &labels ):
        d_labels( labels )
    {
        setTickLength( QwtScaleDiv::MinorTick, 0 );
        setTickLength( QwtScaleDiv::MediumTick, 0 );
        setTickLength( QwtScaleDiv::MajorTick, 2 );

        enableComponent( QwtScaleDraw::Backbone, false );

        if ( orientation == Qt::Vertical )
        {
            setLabelRotation( -60.0 );
        }
        else
        {
            setLabelRotation( -20.0 );
        }

        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    }

    virtual QwtText label( double value ) const
    {
        QwtText lbl;

        const int index = qRound( value );
        if ( index >= 0 && index < d_labels.size() )
        {
            lbl = d_labels[ index ];
        }
            
        return lbl;
    }

private:
    const QStringList d_labels;
};

class DistroChartItem: public QwtPlotBarChart
{
public:
    DistroChartItem():
        QwtPlotBarChart( "Page Hits" )
    {
        setLegendMode( QwtPlotBarChart::LegendBarTitles );
        setLegendIconSize( QSize( 10, 14 ) );
        setLayoutPolicy( AutoAdjustSamples );
        setLayoutHint( 4.0 ); // minimum width for a single bar

        setSpacing( 10 ); // spacing between bars
    }

    void addDistro( const QString &distro, const QColor &color )
    {
        d_colors += color;
        d_distros += distro;
        itemChanged();
    }

    virtual QwtColumnSymbol *specialSymbol(
        int index, const QPointF& ) const
    {
        // we want to have individual colors for each bar

        QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );

        QColor c( Qt::white );
        if ( index >= 0 && index < d_colors.size() )
            c = d_colors[ index ];

        symbol->setPalette( c );
    
        return symbol;
    }

    virtual QwtText barTitle( int sampleIndex ) const
    {
        QwtText title;
        if ( sampleIndex >= 0 && sampleIndex < d_distros.size() )
            title = d_distros[ sampleIndex ];

        return title;
    }

private:
    QList<QColor> d_colors;
    QList<QString> d_distros;
};

BarChart::BarChart( QWidget *parent ):
    QwtPlot( parent )
{
    const struct 
    {
        const char *distro;
        const int hits;
        QColor color;

    } pageHits[] =
    {
        { "Arch", 1114, QColor( "DodgerBlue" ) },
        { "Debian", 1373, QColor( "#d70751" ) },
        { "Fedora", 1638, QColor( "SteelBlue" ) },
        { "Mageia", 1395, QColor( "Indigo" ) },
        { "Mint", 3874, QColor( 183, 255, 183 ) },
        { "openSuSE", 1532, QColor( 115, 186, 37 ) },
        { "Puppy", 1059, QColor( "LightSkyBlue" ) },
        { "Ubuntu", 2391, QColor( "FireBrick" ) }
    };

    setAutoFillBackground( true );
    setPalette( QColor( "Linen" ) );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 2 );
    canvas->setFrameStyle( QFrame::Box | QFrame::Sunken );
    canvas->setBorderRadius( 10 );

    QPalette canvasPalette( QColor( "Plum" ) );
    canvasPalette.setColor( QPalette::Foreground, QColor( "Indigo" ) );
    canvas->setPalette( canvasPalette );

    setCanvas( canvas );

    setTitle( "DistroWatch Page Hit Ranking, April 2012" );

    d_barChartItem = new DistroChartItem();

    QVector< double > samples;

    for ( uint i = 0; i < sizeof( pageHits ) / sizeof( pageHits[ 0 ] ); i++ )
    {
        d_distros += pageHits[ i ].distro;
        samples += pageHits[ i ].hits;

        d_barChartItem->addDistro( 
            pageHits[ i ].distro, pageHits[ i ].color );
    }

    d_barChartItem->setSamples( samples );

    d_barChartItem->attach( this );

    insertLegend( new QwtLegend() );

    setOrientation( 0 );
    setAutoReplot( false );
}

void BarChart::setOrientation( int o )
{
    const Qt::Orientation orientation =
        ( o == 0 ) ? Qt::Vertical : Qt::Horizontal;

    int axis1 = QwtPlot::xBottom;
    int axis2 = QwtPlot::yLeft;

    if ( orientation == Qt::Horizontal )
        qSwap( axis1, axis2 );

    d_barChartItem->setOrientation( orientation );

    setAxisTitle( axis1, "Distros" );
    setAxisMaxMinor( axis1, 3 );
    setAxisScaleDraw( axis1, new DistroScaleDraw( orientation, d_distros ) );

    setAxisTitle( axis2, "Hits per day ( HPD )" );
    setAxisMaxMinor( axis2, 3 );

    QwtScaleDraw *scaleDraw = new QwtScaleDraw();
    scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 4 );
    setAxisScaleDraw( axis2, scaleDraw );

    plotLayout()->setCanvasMargin( 0 );
    replot();
}

void BarChart::exportChart()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "distrowatch.pdf" );
}
