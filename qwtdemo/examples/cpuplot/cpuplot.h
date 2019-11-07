#include <qwt_plot.h>
#include "cpustat.h"

#define HISTORY 60 // seconds

class QwtPlotCurve;

class CpuPlot : public QwtPlot
{
    Q_OBJECT
public:
    enum CpuData
    {
        User,
        System,
        Total,
        Idle,

        NCpuData
    };

    CpuPlot( QWidget * = 0 );
    const QwtPlotCurve *cpuCurve( int id ) const
    {
        return data[id].curve;
    }

protected:
    void timerEvent( QTimerEvent *e );

private Q_SLOTS:
    void legendChecked( const QVariant &, bool on );

private:
    void showCurve( QwtPlotItem *, bool on );

    struct
    {
        QwtPlotCurve *curve;
        double data[HISTORY];
    } data[NCpuData];

    double timeData[HISTORY];

    int dataCount;
    CpuStat cpuStat;
};
