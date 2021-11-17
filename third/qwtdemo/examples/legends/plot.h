#ifndef _PLOT_H_
#define _PLOT_H_

#include <qwt_plot.h>

class Settings;
class LegendItem;
class QwtLegend;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget *parent = NULL );
    virtual ~Plot();

public Q_SLOTS:
    void applySettings( const Settings & );

public:
    virtual void replot();

private:
    void insertCurve();

    QwtLegend *d_externalLegend;
    LegendItem *d_legendItem;
    bool d_isDirty;
};

#endif
