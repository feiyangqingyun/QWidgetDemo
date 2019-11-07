#ifndef _PLOT_H_
#define _PLOT_H_ 1

#include <qwt_plot.h>
#include <qwt_system_clock.h>
#include "settings.h"

class QwtPlotGrid;
class QwtPlotCurve;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget* = NULL );

public Q_SLOTS:
    void setSettings( const Settings & );

protected:
    virtual void timerEvent( QTimerEvent *e );

private:
    void alignScales();

    QwtPlotGrid *d_grid;
    QwtPlotCurve *d_curve;

    QwtSystemClock d_clock;
    double d_interval;

    int d_timerId;

    Settings d_settings;
};

#endif
