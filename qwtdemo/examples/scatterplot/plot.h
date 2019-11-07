#ifndef _PLOT_H_
#define _PLOT_H_ 1

#include <qwt_plot.h>

class QwtPlotCurve;
class QwtSymbol;

class Plot : public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget *parent = NULL );

    void setSymbol( QwtSymbol * );
    void setSamples( const QVector<QPointF> &samples );

private:
    QwtPlotCurve *d_curve;
};

#endif // _PLOT_H_
