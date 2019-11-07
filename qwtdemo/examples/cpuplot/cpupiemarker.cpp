#include <qpainter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include "cpuplot.h"
#include "cpupiemarker.h"

CpuPieMarker::CpuPieMarker()
{
    setZ( 1000 );
    setRenderHint( QwtPlotItem::RenderAntialiased, true );
}

int CpuPieMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}

void CpuPieMarker::draw( QPainter *painter,
    const QwtScaleMap &, const QwtScaleMap &,
    const QRectF &rect ) const
{
    const CpuPlot *cpuPlot = static_cast<CpuPlot *> ( plot() );

    const QwtScaleMap yMap = cpuPlot->canvasMap( QwtPlot::yLeft );

    const int margin = 5;

    QRectF pieRect;
    pieRect.setX( rect.x() + margin );
    pieRect.setY( rect.y() + margin );
    pieRect.setHeight( yMap.transform( 80.0 ) );
    pieRect.setWidth( pieRect.height() );

    const int dataType[] = { CpuPlot::User, CpuPlot::System, CpuPlot::Idle };

    int angle = static_cast<int>( 5760 * 0.75 );
    for ( unsigned int i = 0;
        i < sizeof( dataType ) / sizeof( dataType[0] ); i++ )
    {
        const QwtPlotCurve *curve = cpuPlot->cpuCurve( dataType[i] );
        if ( curve->dataSize() > 0 )
        {
            const int value = static_cast<int>( 5760 * curve->sample( 0 ).y() / 100.0 );

            painter->save();
            painter->setBrush( QBrush( curve->brush().color(), Qt::SolidPattern ) );
            if ( value != 0 )
                painter->drawPie( pieRect, -angle, -value );
            painter->restore();

            angle += value;
        }
    }
}
