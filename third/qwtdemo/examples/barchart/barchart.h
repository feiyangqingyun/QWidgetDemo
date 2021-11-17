#ifndef _BAR_CHART_H_

#include <qwt_plot.h>

class QwtPlotMultiBarChart;

class BarChart: public QwtPlot
{
    Q_OBJECT

public:
    BarChart( QWidget * = NULL );

public Q_SLOTS:
    void setMode( int );
    void setOrientation( int );
    void exportChart();

private:
    void populate();

    QwtPlotMultiBarChart *d_barChartItem;
};

#endif
