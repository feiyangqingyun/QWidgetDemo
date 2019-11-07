#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include "cpupiemarker.h"
#include "cpuplot.h"

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addSecs( static_cast<int>( v ) );
        return upTime.toString();
    }
private:
    QTime baseTime;
};

class Background: public QwtPlotItem
{
public:
    Background()
    {
        setZ( 0.0 );
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw( QPainter *painter,
        const QwtScaleMap &, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const
    {
        QColor c( Qt::white );
        QRectF r = canvasRect;

        for ( int i = 100; i > 0; i -= 10 )
        {
            r.setBottom( yMap.transform( i - 10 ) );
            r.setTop( yMap.transform( i ) );
            painter->fillRect( r, c );

            c = c.dark( 110 );
        }
    }
};

class CpuCurve: public QwtPlotCurve
{
public:
    CpuCurve( const QString &title ):
        QwtPlotCurve( title )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;
        c.setAlpha( 150 );

        setPen( QPen( Qt::NoPen ) );
        setBrush( c );
    }
};

CpuPlot::CpuPlot( QWidget *parent ):
    QwtPlot( parent ),
    dataCount( 0 )
{
    setAutoReplot( false );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    setCanvas( canvas );

    plotLayout()->setAlignCanvasToScales( true );

    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    insertLegend( legend, QwtPlot::RightLegend );

    setAxisTitle( QwtPlot::xBottom, " System Uptime [h:m:s]" );
    setAxisScaleDraw( QwtPlot::xBottom,
        new TimeScaleDraw( cpuStat.upTime() ) );
    setAxisScale( QwtPlot::xBottom, 0, HISTORY );
    setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
    setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    /*
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget *scaleWidget = axisWidget( QwtPlot::xBottom );
    const int fmh = QFontMetrics( scaleWidget->font() ).height();
    scaleWidget->setMinBorderDist( 0, fmh / 2 );

    setAxisTitle( QwtPlot::yLeft, "Cpu Usage [%]" );
    setAxisScale( QwtPlot::yLeft, 0, 100 );

    Background *bg = new Background();
    bg->attach( this );

    CpuPieMarker *pie = new CpuPieMarker();
    pie->attach( this );

    CpuCurve *curve;

    curve = new CpuCurve( "System" );
    curve->setColor( Qt::red );
    curve->attach( this );
    data[System].curve = curve;

    curve = new CpuCurve( "User" );
    curve->setColor( Qt::blue );
    curve->setZ( curve->z() - 1 );
    curve->attach( this );
    data[User].curve = curve;

    curve = new CpuCurve( "Total" );
    curve->setColor( Qt::black );
    curve->setZ( curve->z() - 2 );
    curve->attach( this );
    data[Total].curve = curve;

    curve = new CpuCurve( "Idle" );
    curve->setColor( Qt::darkCyan );
    curve->setZ( curve->z() - 3 );
    curve->attach( this );
    data[Idle].curve = curve;

    showCurve( data[System].curve, true );
    showCurve( data[User].curve, true );
    showCurve( data[Total].curve, false );
    showCurve( data[Idle].curve, false );

    for ( int i = 0; i < HISTORY; i++ )
        timeData[HISTORY - 1 - i] = i;

    ( void )startTimer( 1000 ); // 1 second

    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
        SLOT( legendChecked( const QVariant &, bool ) ) );
}

void CpuPlot::timerEvent( QTimerEvent * )
{
    for ( int i = dataCount; i > 0; i-- )
    {
        for ( int c = 0; c < NCpuData; c++ )
        {
            if ( i < HISTORY )
                data[c].data[i] = data[c].data[i-1];
        }
    }

    cpuStat.statistic( data[User].data[0], data[System].data[0] );

    data[Total].data[0] = data[User].data[0] + data[System].data[0];
    data[Idle].data[0] = 100.0 - data[Total].data[0];

    if ( dataCount < HISTORY )
        dataCount++;

    for ( int j = 0; j < HISTORY; j++ )
        timeData[j]++;

    setAxisScale( QwtPlot::xBottom,
        timeData[HISTORY - 1], timeData[0] );

    for ( int c = 0; c < NCpuData; c++ )
    {
        data[c].curve->setRawSamples(
            timeData, data[c].data, dataCount );
    }

    replot();
}

void CpuPlot::legendChecked( const QVariant &itemInfo, bool on )
{
    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if ( plotItem )
        showCurve( plotItem, on );
}

void CpuPlot::showCurve( QwtPlotItem *item, bool on )
{
    item->setVisible( on );

    QwtLegend *lgd = qobject_cast<QwtLegend *>( legend() );

    QList<QWidget *> legendWidgets = 
        lgd->legendWidgets( itemToInfo( item ) );

    if ( legendWidgets.size() == 1 )
    {
        QwtLegendLabel *legendLabel =
            qobject_cast<QwtLegendLabel *>( legendWidgets[0] );

        if ( legendLabel )
            legendLabel->setChecked( on );
    }

    replot();
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QWidget vBox;
    vBox.setWindowTitle( "Cpu Plot" );

    CpuPlot *plot = new CpuPlot( &vBox );
    plot->setTitle( "History" );

    const int margin = 5;
    plot->setContentsMargins( margin, margin, margin, margin );

    QString info( "Press the legend to en/disable a curve" );

    QLabel *label = new QLabel( info, &vBox );

    QVBoxLayout *layout = new QVBoxLayout( &vBox );
    layout->addWidget( plot );
    layout->addWidget( label );

    vBox.resize( 600, 400 );
    vBox.show();

    return a.exec();
}

