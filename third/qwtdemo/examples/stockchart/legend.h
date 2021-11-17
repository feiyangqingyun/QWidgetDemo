#ifndef _LEGEND_H_
#define _LEGEND_H_

#include <qwt_abstract_legend.h>

class LegendTreeView;
class QStandardItem;
class QModelIndex;
class QwtPlotItem;

class Legend : public QwtAbstractLegend
{
    Q_OBJECT

public:
    explicit Legend( QWidget *parent = NULL );
    virtual ~Legend();

    virtual void renderLegend( QPainter *,
        const QRectF &, bool fillBackground ) const;

    virtual bool isEmpty() const;

    virtual int scrollExtent( Qt::Orientation ) const;

Q_SIGNALS:
    void checked( QwtPlotItem *plotItem, bool on, int index );

public Q_SLOTS:
    virtual void updateLegend( const QVariant &,
        const QList<QwtLegendData> & );

private Q_SLOTS:
    void handleClick( const QModelIndex & );

private:
    void updateItem( QStandardItem *, const QwtLegendData & );

    LegendTreeView *d_treeView;
};

#endif
