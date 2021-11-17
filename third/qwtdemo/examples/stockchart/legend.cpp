#include "legend.h"
#include <qwt_legend_data.h>
#include <qwt_text.h>
#include <qwt_plot_item.h>
#include <qtreeview.h>
#include <qlayout.h>
#include <qstyle.h>
#include <qstandarditemmodel.h>
#include <qitemdelegate.h>
#include <qpainter.h>

static void qwtRenderBackground( QPainter *painter,
    const QRectF &rect, const QWidget *widget )
{
    if ( widget->testAttribute( Qt::WA_StyledBackground ) )
    {
        QStyleOption opt;
        opt.initFrom( widget );
        opt.rect = rect.toAlignedRect();

        widget->style()->drawPrimitive(
            QStyle::PE_Widget, &opt, painter, widget);
    }
    else
    {
        const QBrush brush =
            widget->palette().brush( widget->backgroundRole() );

        painter->fillRect( rect, brush );
    }
}

class LegendTreeView: public QTreeView
{
public:
    LegendTreeView( Legend * );

    QStandardItem *rootItem( int rtti );
    QStandardItem *insertRootItem( int rtti );

    QList<QStandardItem *> itemList( const QwtPlotItem * );

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
};

LegendTreeView::LegendTreeView( Legend *legend ):
    QTreeView( legend )
{
    setFrameStyle( NoFrame );
    viewport()->setBackgroundRole(QPalette::Background);
    viewport()->setAutoFillBackground( false );

    setRootIsDecorated( true );
    setHeaderHidden( true );

    QStandardItemModel *model = new QStandardItemModel();

    setModel( model );

    // we want unstyled items
    setItemDelegate( new QItemDelegate( this ) );
}

QStandardItem *LegendTreeView::rootItem( int rtti )
{
    QStandardItemModel *mdl =
        qobject_cast<QStandardItemModel *>( model() );

    for ( int row = 0; row < mdl->rowCount(); row++ )
    {
        QStandardItem *item = mdl->item( row );
        if ( item->data() == rtti )
            return item;
    }

    return NULL;
}

QList<QStandardItem *> LegendTreeView::itemList( 
    const QwtPlotItem *plotItem ) 
{
    QList<QStandardItem *> itemList;

    const QStandardItem *rootItem = this->rootItem( plotItem->rtti() );
    if ( rootItem )
    {
        for ( int i = 0; i < rootItem->rowCount(); i++ )
        {
            QStandardItem *item = rootItem->child( i );
        
            const QVariant key = item->data();
        
            if ( key.canConvert<qlonglong>() )
            {
                const qlonglong ptr = key.value<qlonglong>();
                if ( ptr == qlonglong( plotItem ) )
                    itemList += item;
            }
        }
    }

    return itemList;
}

QStandardItem *LegendTreeView::insertRootItem( int rtti )
{
    QStandardItem *item = new QStandardItem();
    item->setEditable( false );
    item->setData( rtti );

    switch( rtti )
    {
        case QwtPlotItem::Rtti_PlotTradingCurve:
        {
            item->setText( "Curves" );
            break;
        }
        case QwtPlotItem::Rtti_PlotZone:
        {
            item->setText( "Zones" );
            break;
        }
        case QwtPlotItem::Rtti_PlotMarker:
        {
            item->setText( "Events" );
            break;
        }
        default:
            break;
    }

    QStandardItemModel *mdl =
        qobject_cast<QStandardItemModel *>( model() );

    mdl->appendRow( item );
    setExpanded( mdl->index( mdl->rowCount() - 1, 0 ), true );

    return item;
}

QSize LegendTreeView::minimumSizeHint() const
{
    return QSize( -1, -1 );
}

QSize LegendTreeView::sizeHint() const
{
    QStyleOptionViewItem styleOption;
    styleOption.initFrom( this );

    const QAbstractItemDelegate *delegate = itemDelegate();

    const QStandardItemModel *mdl =
        qobject_cast<const QStandardItemModel *>( model() );

    int w = 0;
    int h = 0;

    for ( int row = 0; row < mdl->rowCount(); row++ )
    {
        const QStandardItem *rootItem = mdl->item( row );

        int wRow = 0;
        for ( int i = 0; i < rootItem->rowCount(); i++ )
        {
            const QSize hint = delegate->sizeHint( styleOption, 
                rootItem->child( i )->index() );

            wRow = qMax( wRow, hint.width() );
            h += hint.height();
        }

        const QSize rootHint = delegate->sizeHint( 
            styleOption, rootItem->index() );

        wRow = qMax( wRow + indentation(), rootHint.width() );
        if ( wRow > w )
            w = wRow;

        if ( rootIsDecorated() )
            w += indentation();

        h += rootHint.height();
    }

    int left, right, top, bottom;
    getContentsMargins( &left, &top, &right, &bottom );

    w += left + right;
    h += top + bottom;

    return QSize( w, h );
}

Legend::Legend( QWidget *parent ):
    QwtAbstractLegend( parent )
{
    d_treeView = new LegendTreeView( this );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( d_treeView );

    connect( d_treeView, SIGNAL( clicked( const QModelIndex & ) ),
        this, SLOT( handleClick( const QModelIndex & ) ) );
}

Legend::~Legend()
{
}

void Legend::renderLegend( QPainter *painter,
    const QRectF &rect, bool fillBackground ) const
{
    if ( fillBackground )
    {
        if ( autoFillBackground() ||
            testAttribute( Qt::WA_StyledBackground ) )
        {
            qwtRenderBackground( painter, rect, d_treeView );
        }
    }

    QStyleOptionViewItem styleOption;
    styleOption.initFrom( this );
    styleOption.decorationAlignment = Qt::AlignCenter;

    const QAbstractItemDelegate *delegate = d_treeView->itemDelegate();

    const QStandardItemModel *mdl =
        qobject_cast<const QStandardItemModel *>( d_treeView->model() );

    painter->save();
    painter->translate( rect.topLeft() );

    for ( int row = 0; row < mdl->rowCount(); row++ )
    {
        const QStandardItem *rootItem = mdl->item( row );

        styleOption.rect = d_treeView->visualRect( rootItem->index() );
        if ( !styleOption.rect.isEmpty() )
            delegate->paint( painter, styleOption, rootItem->index() );

        for ( int i = 0; i < rootItem->rowCount(); i++ )
        {
            const QStandardItem *item = rootItem->child( i );

            styleOption.rect = d_treeView->visualRect( item->index() );
            if ( !styleOption.rect.isEmpty() )
            {
                delegate->paint( painter, styleOption, item->index() );
            }
        }
    }
    painter->restore();
}

bool Legend::isEmpty() const
{
    return d_treeView->model()->rowCount() == 0;
}

int Legend::scrollExtent( Qt::Orientation orientation ) const
{
    Q_UNUSED( orientation );

    return style()->pixelMetric( QStyle::PM_ScrollBarExtent );
}

void Legend::updateLegend( const QVariant &itemInfo,
    const QList<QwtLegendData> &data )
{
    QwtPlotItem *plotItem = qvariant_cast<QwtPlotItem *>( itemInfo );

    QStandardItem *rootItem = d_treeView->rootItem( plotItem->rtti() );
    QList<QStandardItem *> itemList = d_treeView->itemList( plotItem );

    while ( itemList.size() > data.size() )
    {
        QStandardItem *item = itemList.takeLast();
        rootItem->removeRow( item->row() );
    }

    if ( !data.isEmpty() )
    {
        if ( rootItem == NULL )
            rootItem = d_treeView->insertRootItem( plotItem->rtti() );

        while ( itemList.size() < data.size() )
        {
            QStandardItem *item = new QStandardItem();
            item->setEditable( false );
            item->setData( qlonglong( plotItem ) );
            item->setCheckable( true );
            item->setCheckState( plotItem->isVisible() ?
                Qt::Checked : Qt::Unchecked );

            itemList += item;
            rootItem->appendRow( item );
        }

        for ( int i = 0; i < itemList.size(); i++ )
            updateItem( itemList[i], data[i] );
    }
    else
    {
        if ( rootItem && rootItem->rowCount() == 0 )
            d_treeView->model()->removeRow( rootItem->row() );
    }

    d_treeView->updateGeometry();
}

void Legend::updateItem( QStandardItem *item, const QwtLegendData &data )
{
    const QVariant titleValue = data.value( QwtLegendData::TitleRole );

    QwtText title;
    if ( titleValue.canConvert<QwtText>() )
    {
        item->setText( title.text() );
        title = titleValue.value<QwtText>();
    }
    else if ( titleValue.canConvert<QString>() )
    {
        title.setText( titleValue.value<QString>() );
    }
    item->setText( title.text() );

    const QVariant iconValue = data.value( QwtLegendData::IconRole );

    QPixmap pm;
    if ( iconValue.canConvert<QPixmap>() )
        pm = iconValue.value<QPixmap>();

    item->setData(pm, Qt::DecorationRole);
}

void Legend::handleClick( const QModelIndex &index )
{
    const QStandardItemModel *model =
        qobject_cast<QStandardItemModel *>( d_treeView->model() );

    const QStandardItem *item = model->itemFromIndex( index );
    if ( item->isCheckable() )
    {
        const qlonglong ptr = item->data().value<qlonglong>();
    
        Q_EMIT checked( (QwtPlotItem *)ptr, 
            item->checkState() == Qt::Checked, 0 );
    }
}
