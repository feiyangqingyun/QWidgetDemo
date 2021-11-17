#ifndef _GRID_ITEM_H_
#define _GRID_ITEM_H_

#include <qwt_plot_item.h>
#include <qwt_scale_div.h>
#include <qpalette.h>

class GridItem: public QwtPlotItem
{
public:
    enum GridAttribute
    {
        AutoUpdate = 0x01,
        FillCanvas       = 0x02
    };

    typedef QFlags<GridAttribute> GridAttributes;

    explicit GridItem();
    virtual ~GridItem();

    virtual int rtti() const;

    void setGridAttribute( GridAttribute, bool on = true );
    bool testGridAttribute( GridAttribute ) const;

    void setOrientations( Qt::Orientations );
    Qt::Orientations orientations() const;

    void enableXMin( bool );
    bool isXMinEnabled() const;

    void enableYMin( bool );
    bool isYMinEnabled() const;

    void setXDiv( const QwtScaleDiv &sx );
    const QwtScaleDiv &xScaleDiv() const;

    void setYDiv( const QwtScaleDiv &sy );
    const QwtScaleDiv &yScaleDiv() const;

    void setPalette( const QPalette & );
    QPalette palette() const;

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &rect ) const;

    virtual void updateScaleDiv(
        const QwtScaleDiv &xMap, const QwtScaleDiv &yMap );

protected:
    virtual QBrush brush( int row, int column, const QRectF & ) const;

private:
    Qt::Orientations m_orientations;
    GridAttributes m_gridAttributes;

    QwtScaleDiv m_xScaleDiv;
    QwtScaleDiv m_yScaleDiv;

    bool m_isXMinEnabled;
    bool m_isYMinEnabled;

    QPalette m_palette;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( GridItem::GridAttributes )

#endif
