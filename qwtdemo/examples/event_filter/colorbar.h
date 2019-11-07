#include <qwidget.h>

class ColorBar: public QWidget
{
    Q_OBJECT

public:
    ColorBar( Qt::Orientation = Qt::Horizontal, QWidget * = NULL );

    virtual void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const { return d_orientation; }

    void setRange( const QColor &light, const QColor &dark );
    void setLight( const QColor &light );
    void setDark( const QColor &dark );

    QColor light() const { return d_light; }
    QColor dark() const { return d_dark; }

Q_SIGNALS:
    void selected( const QColor & );

protected:
    virtual void mousePressEvent( QMouseEvent * );
    virtual void paintEvent( QPaintEvent * );

    void drawColorBar( QPainter *, const QRect & ) const;

private:
    Qt::Orientation d_orientation;
    QColor d_light;
    QColor d_dark;
};
