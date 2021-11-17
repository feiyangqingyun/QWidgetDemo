#include <qobject.h>
#include <qrect.h>

class QwtPlot;
class QwtScaleWidget;

class ScalePicker: public QObject
{
    Q_OBJECT
public:
    ScalePicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

Q_SIGNALS:
    void clicked( int axis, double value );

private:
    void mouseClicked( const QwtScaleWidget *, const QPoint & );
    QRect scaleRect( const QwtScaleWidget * ) const;
};
