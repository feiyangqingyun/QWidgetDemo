#include "plot.h"
#include "settings.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_legenditem.h>
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>

class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );

        QColor color( Qt::white );

        setTextPen( color );
#if 1
        setBorderPen( color );

        QColor c( Qt::gray );
        c.setAlpha( 200 );

        setBackgroundBrush( c );
#endif
    }
};

class Curve: public QwtPlotCurve
{
public:
    Curve( int index ):
        d_index( index )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
        initData();
    }

    void setCurveTitle( const QString &title )
    {
        QString txt("%1 %2");
        setTitle( QString( "%1 %2" ).arg( title ).arg( d_index ) );
    }

    void initData()
    {
        QVector<QPointF> points;

        double y = qrand() % 1000;

        for ( double x = 0.0; x <= 1000.0; x += 100.0 )
        {
            double off = qrand() % 200 - 100;
            if ( y + off > 980.0 || y + off < 20.0 )
                off = -off;

            y += off;

            points += QPointF( x, y );
        }

        setSamples( points );
    }

private:
    const int d_index;
};

Plot::Plot( QWidget *parent ):
    QwtPlot( parent ),
    d_externalLegend( NULL ),
    d_legendItem( NULL ),
    d_isDirty( false )
{
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFocusIndicator( QwtPlotCanvas::CanvasFocusIndicator );
    canvas->setFocusPolicy( Qt::StrongFocus );
    canvas->setPalette( Qt::black );
    setCanvas( canvas );

    setAutoReplot( false );

    setTitle( "Legend Test" );
    setFooter( "Footer" );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
    grid->attach( this );

    // axis
    setAxisScale( QwtPlot::yLeft, 0.0, 1000.0 );
    setAxisScale( QwtPlot::xBottom, 0.0, 1000.0 );
}

Plot::~Plot()
{
    delete d_externalLegend;
}

void Plot::insertCurve()
{
    static int counter = 1;

    const char *colors[] = 
    { 
        "LightSalmon",
        "SteelBlue",
        "Yellow",
        "Fuchsia",
        "PaleGreen",
        "PaleTurquoise",
        "Cornsilk",
        "HotPink",
        "Peru",
        "Maroon"
    };
    const int numColors = sizeof( colors ) / sizeof( colors[0] );

    QwtPlotCurve *curve = new Curve( counter++ );
    curve->setPen( QColor( colors[ counter % numColors ] ), 2 );
    curve->attach( this );
}

void Plot::applySettings( const Settings &settings )
{
    d_isDirty = false;
    setAutoReplot( true );

    if ( settings.legend.isEnabled )
    {
        if ( settings.legend.position > QwtPlot::TopLegend )
        {
            if ( legend() )
            {
                // remove legend controlled by the plot
                insertLegend( NULL );
            }

            if ( d_externalLegend == NULL )
            {
                d_externalLegend = new QwtLegend();
                d_externalLegend->setWindowTitle("Plot Legend");

                connect( 
                    this, 
                    SIGNAL( legendDataChanged( const QVariant &, 
                        const QList<QwtLegendData> & ) ),
                    d_externalLegend, 
                    SLOT( updateLegend( const QVariant &, 
                        const QList<QwtLegendData> & ) ) );

                d_externalLegend->show();

                // populate the new legend
                updateLegend();
            }
        }
        else
        {
            delete d_externalLegend;
            d_externalLegend = NULL;

            if ( legend() == NULL || 
                plotLayout()->legendPosition() != settings.legend.position )
            {
                insertLegend( new QwtLegend(), 
                    QwtPlot::LegendPosition( settings.legend.position ) );
            }
        }
    }
    else
    {
        insertLegend( NULL );

        delete d_externalLegend;
        d_externalLegend = NULL;
    }

    if ( settings.legendItem.isEnabled )
    {
        if ( d_legendItem == NULL )
        {
            d_legendItem = new LegendItem();
            d_legendItem->attach( this );
        }

        d_legendItem->setMaxColumns( settings.legendItem.numColumns );
        d_legendItem->setAlignment( Qt::Alignment( settings.legendItem.alignment ) );
        d_legendItem->setBackgroundMode(
            QwtPlotLegendItem::BackgroundMode( settings.legendItem.backgroundMode ) );
        if ( settings.legendItem.backgroundMode == 
            QwtPlotLegendItem::ItemBackground )
        {
            d_legendItem->setBorderRadius( 4 );
            d_legendItem->setMargin( 0 );
            d_legendItem->setSpacing( 4 );
            d_legendItem->setItemMargin( 2 );
        }
        else
        {
            d_legendItem->setBorderRadius( 8 );
            d_legendItem->setMargin( 4 );
            d_legendItem->setSpacing( 2 );
            d_legendItem->setItemMargin( 0 );
        }

        QFont font = d_legendItem->font();
        font.setPointSize( settings.legendItem.size );
        d_legendItem->setFont( font );
    }
    else
    {
        delete d_legendItem;
        d_legendItem = NULL;
    }

    QwtPlotItemList curveList = itemList( QwtPlotItem::Rtti_PlotCurve );
    if ( curveList.size() != settings.curve.numCurves )
    {
        while ( curveList.size() > settings.curve.numCurves )
        {
            QwtPlotItem* curve = curveList.takeFirst();
            delete curve;
        }
        
        for ( int i = curveList.size(); i < settings.curve.numCurves; i++ )
            insertCurve();
    }

    curveList = itemList( QwtPlotItem::Rtti_PlotCurve );
    for ( int i = 0; i < curveList.count(); i++ )
    {
        Curve* curve = static_cast<Curve*>( curveList[i] );
        curve->setCurveTitle( settings.curve.title );

        int sz = 0.5 * settings.legendItem.size;
        curve->setLegendIconSize( QSize( sz, sz ) );
    }

    setAutoReplot( false );
    if ( d_isDirty )
    {
        d_isDirty = false;
        replot();
    }
}

void Plot::replot()
{
    if ( autoReplot() )
    {
        d_isDirty = true;
        return;
    }

    QwtPlot::replot();
}

