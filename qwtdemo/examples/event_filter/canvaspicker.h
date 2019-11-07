#include <qobject.h>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;

class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    CanvasPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

    virtual bool event( QEvent * );

private:
    void select( const QPoint & );
    void move( const QPoint & );
    void moveBy( int dx, int dy );

    void release();

    void showCursor( bool enable );
    void shiftPointCursor( bool up );
    void shiftCurveCursor( bool up );

    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
};
