#include <qwt_plot.h>
#include <qdatetime.h>

class Curve;

class Plot: public QwtPlot
{
public:
    Plot( QWidget * = NULL);

protected:
    virtual void timerEvent( QTimerEvent * );

private:
    void updateCurves();

    enum { CurveCount = 4 };
    Curve *d_curves[CurveCount];

    QTime d_time;
};
