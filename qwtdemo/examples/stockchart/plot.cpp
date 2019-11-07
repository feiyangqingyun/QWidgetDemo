#include "plot.h"
#include "legend.h"
#include "griditem.h"
#include "quotefactory.h"
#include <qwt_legend.h>
#include <qwt_plot_tradingcurve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_legend_label.h>
#include <qwt_date.h>
#include <qwt_date_scale_engine.h>
#include <qwt_date_scale_draw.h>

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setRubberBandPen( QColor( Qt::darkGreen ) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
    }

protected:
    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        const QDateTime dt = QwtDate::toDateTime( pos.x() );

        QString s;
        s += QwtDate::toString( QwtDate::toDateTime( pos.x() ),
            "MMM dd hh:mm ", QwtDate::FirstThursday );

        QwtText text( s );
        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
};

class DateScaleDraw: public QwtDateScaleDraw
{
public:
    DateScaleDraw( Qt::TimeSpec timeSpec ):
        QwtDateScaleDraw( timeSpec )
    {
        // as we have dates from 2010 only we use
        // format strings without the year

        setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz\nddd dd MMM" );
        setDateFormat( QwtDate::Second, "hh:mm:ss\nddd dd MMM" );
        setDateFormat( QwtDate::Minute, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Hour, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Day, "ddd dd MMM" );
        setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
};

class ZoneItem: public QwtPlotZoneItem
{
public:
    ZoneItem( const QString &title )
    {
        setTitle( title );
        setZ( 11 ); // on top the the grid
        setOrientation( Qt::Vertical );
        setItemAttribute( QwtPlotItem::Legend, true );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;

        c.setAlpha( 100 );
        setPen( c );

        c.setAlpha( 20 );
        setBrush( c );
    }

    void setInterval( const QDate &date1, const QDate &date2 )
    {
        const QDateTime dt1( date1, QTime(), Qt::UTC );
        const QDateTime dt2( date2, QTime(), Qt::UTC );

        QwtPlotZoneItem::setInterval( QwtDate::toDouble( dt1 ),
            QwtDate::toDouble( dt2 ) );
    }
};

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    setTitle( "Trading Chart" );

    QwtDateScaleDraw *scaleDraw = new DateScaleDraw( Qt::UTC );
    QwtDateScaleEngine *scaleEngine = new QwtDateScaleEngine( Qt::UTC );

    setAxisTitle( QwtPlot::xBottom, QString( "2010" ) );
    setAxisScaleDraw( QwtPlot::xBottom, scaleDraw );
    setAxisScaleEngine( QwtPlot::xBottom, scaleEngine );
    setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
    setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    setAxisTitle( QwtPlot::yLeft, QString( "Price [EUR]" ) );

#if 0
    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    insertLegend( legend, QwtPlot::RightLegend );
#else
    Legend *legend = new Legend;
    insertLegend( legend, QwtPlot::RightLegend );
#endif

    populate();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    Zoomer* zoomer = new Zoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setMouseButton( Qt::MidButton );

    connect( legend, SIGNAL( checked( QwtPlotItem *, bool, int ) ),
        SLOT( showItem( QwtPlotItem *, bool ) ) );
}

void Plot::populate()
{
    GridItem *gridItem = new GridItem();
#if 0
    gridItem->setOrientations( Qt::Horizontal );
#endif
    gridItem->attach( this );

    const Qt::GlobalColor colors[] =
    {
        Qt::red,
        Qt::blue,
        Qt::darkCyan,
        Qt::darkMagenta,
        Qt::darkYellow
    };

    const int numColors = sizeof( colors ) / sizeof( colors[0] );

    for ( int i = 0; i < QuoteFactory::NumStocks; i++ )
    {
        QuoteFactory::Stock stock = static_cast<QuoteFactory::Stock>( i );

        QwtPlotTradingCurve *curve = new QwtPlotTradingCurve();
        curve->setTitle( QuoteFactory::title( stock ) );
        curve->setOrientation( Qt::Vertical );
        curve->setSamples( QuoteFactory::samples2010( stock ) );

        // as we have one sample per day a symbol width of
        // 12h avoids overlapping symbols. We also bound
        // the width, so that is is not scaled below 3 and
        // above 15 pixels.

        curve->setSymbolExtent( 12 * 3600 * 1000.0 );
        curve->setMinSymbolWidth( 3 );
        curve->setMaxSymbolWidth( 15 );

        const Qt::GlobalColor color = colors[ i % numColors ];

        curve->setSymbolPen( color );
        curve->setSymbolBrush( QwtPlotTradingCurve::Decreasing, color );
        curve->setSymbolBrush( QwtPlotTradingCurve::Increasing, Qt::white );
        curve->attach( this );

        showItem( curve, true );
    }

    for ( int i = 0; i < 2; i++ )
    {
        QwtPlotMarker *marker = new QwtPlotMarker();

        marker->setTitle( QString( "Event %1" ).arg( i + 1 ) );
        marker->setLineStyle( QwtPlotMarker::VLine );
        marker->setLinePen( colors[ i % numColors ], 0, Qt::DashLine );
        marker->setVisible( false );

        QDateTime dt( QDate( 2010, 1, 1 ) );
        dt = dt.addDays( 77 * ( i + 1 ) );
        
        marker->setValue( QwtDate::toDouble( dt ), 0.0 );

        marker->setItemAttribute( QwtPlotItem::Legend, true );

        marker->attach( this );
    }

    // to show how QwtPlotZoneItem works

    ZoneItem *zone1 = new ZoneItem( "Zone 1");
    zone1->setColor( Qt::darkBlue );
    zone1->setInterval( QDate( 2010, 3, 10 ), QDate( 2010, 3, 27 ) );
    zone1->setVisible( false );
    zone1->attach( this );

    ZoneItem *zone2 = new ZoneItem( "Zone 2");
    zone2->setColor( Qt::darkMagenta );
    zone2->setInterval( QDate( 2010, 8, 1 ), QDate( 2010, 8, 24 ) );
    zone2->setVisible( false );
    zone2->attach( this );

}

void Plot::setMode( int style )
{
    QwtPlotTradingCurve::SymbolStyle symbolStyle =
        static_cast<QwtPlotTradingCurve::SymbolStyle>( style );

    QwtPlotItemList curves = itemList( QwtPlotItem::Rtti_PlotTradingCurve );
    for ( int i = 0; i < curves.size(); i++ )
    {
        QwtPlotTradingCurve *curve =
            static_cast<QwtPlotTradingCurve *>( curves[i] );
        curve->setSymbolStyle( symbolStyle );
    }

    replot();
}

void Plot::showItem( QwtPlotItem *item, bool on )
{
    item->setVisible( on );
    replot();
}

void Plot::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( this, "stockchart.pdf" );
}
