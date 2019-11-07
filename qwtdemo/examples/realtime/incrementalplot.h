#ifndef _INCREMENTALPLOT_H_
#define _INCREMENTALPLOT_H_ 1

#include <qwt_plot.h>

class QwtPlotCurve;
class QwtPlotDirectPainter;

class IncrementalPlot : public QwtPlot
{
    Q_OBJECT

public:
    IncrementalPlot( QWidget *parent = NULL );
    virtual ~IncrementalPlot();

    void appendPoint( const QPointF & );
    void clearPoints();

public Q_SLOTS:
    void showSymbols( bool );

private:
    QwtPlotCurve *d_curve;
    QwtPlotDirectPainter *d_directPainter;
};

#endif // _INCREMENTALPLOT_H_
