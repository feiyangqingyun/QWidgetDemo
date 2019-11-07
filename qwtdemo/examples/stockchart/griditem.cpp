#include "griditem.h"
#include <qwt_scale_map.h>
#include <qwt_painter.h>
#include <qpainter.h>

GridItem::GridItem():
    QwtPlotItem( QwtText( "Grid" ) ),
    m_orientations( Qt::Horizontal | Qt::Vertical ),
    m_gridAttributes( AutoUpdate | FillCanvas ),
    m_isXMinEnabled( false ),
    m_isYMinEnabled( false )
{
    setItemInterest( QwtPlotItem::ScaleInterest, true );
    setZ( 10.0 );
}

GridItem::~GridItem()
{
}

int GridItem::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem + 99; // something
}

void GridItem::setGridAttribute( GridAttribute attribute, bool on )
{
    if ( bool( m_gridAttributes & attribute ) == on )
        return;

    if ( on )
        m_gridAttributes |= attribute;
    else
        m_gridAttributes &= ~attribute;

    itemChanged();
}

bool GridItem::testGridAttribute( GridAttribute attribute ) const
{
    return m_gridAttributes & attribute;
}

void GridItem::setOrientations( Qt::Orientations orientations )
{
    if ( m_orientations != orientations )
    {
        m_orientations = orientations;
        itemChanged();
    }
}

Qt::Orientations GridItem::orientations() const
{
    return m_orientations;
}

void GridItem::enableXMin( bool enabled )
{
    if ( enabled != m_isXMinEnabled )
    {
        m_isXMinEnabled = enabled;
        itemChanged();
    }
}

bool GridItem::isXMinEnabled() const
{
    return m_isXMinEnabled;
}

void GridItem::enableYMin( bool enabled )
{
    if ( enabled != m_isYMinEnabled )
    {
        m_isYMinEnabled = enabled;
        itemChanged();
    }
}

bool GridItem::isYMinEnabled() const
{
    return m_isYMinEnabled;
}

void GridItem::setXDiv( const QwtScaleDiv &scaleDiv )
{
    if ( m_xScaleDiv != scaleDiv )
    {
        m_xScaleDiv = scaleDiv;
        itemChanged();
    }
}

void GridItem::setYDiv( const QwtScaleDiv &scaleDiv )
{
    if ( m_yScaleDiv != scaleDiv )
    {
        m_yScaleDiv = scaleDiv;
        itemChanged();
    }
}

void GridItem::setPalette( const QPalette &palette )
{
    if ( m_palette != palette )
    {
        m_palette = palette;
        itemChanged();
    }
}

QPalette GridItem::palette() const
{
    return m_palette;
}

void GridItem::draw( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect ) const
{
    const bool doAlign = QwtPainter::roundingAlignment( painter );

    const QRectF area = QwtScaleMap::invTransform( xMap, yMap, canvasRect );

    QList<double> xValues;
    if ( m_orientations & Qt::Horizontal )
    {
        xValues = m_xScaleDiv.ticks( QwtScaleDiv::MajorTick );

        if ( m_isXMinEnabled )
        {
            xValues += m_xScaleDiv.ticks( QwtScaleDiv::MediumTick );
            xValues += m_xScaleDiv.ticks( QwtScaleDiv::MinorTick );
        }

        if ( m_gridAttributes & FillCanvas )
        {
            xValues += area.left();
            xValues += area.right();
        }

        qSort( xValues );
    }

    QList<double> yValues;
    if ( m_orientations & Qt::Vertical )
    {
        yValues = m_yScaleDiv.ticks( QwtScaleDiv::MajorTick );

        if ( m_isYMinEnabled )
        {
            yValues += m_yScaleDiv.ticks( QwtScaleDiv::MediumTick );
            yValues += m_yScaleDiv.ticks( QwtScaleDiv::MinorTick );
        }

        if ( m_gridAttributes & FillCanvas )
        {
            yValues += area.top();
            yValues += area.bottom();
        }

        qSort( yValues );
    }

    painter->setPen( Qt::NoPen );

    if ( ( m_orientations & Qt::Horizontal ) &&
        ( m_orientations & Qt::Vertical ) )
    {
        for ( int i = 1; i < xValues.size(); i++ )
        {
            double x1 = xMap.transform( xValues[i - 1] );
            double x2 = xMap.transform( xValues[i] );

            if ( doAlign )
            {
                x1 = qRound( x1 );
                x2 = qRound( x2 );
            }

            for ( int j = 1; j < yValues.size(); j++ )
            {
                const QRectF rect( xValues[i - 1], yValues[j - 1],
                    xValues[i] - xValues[i - 1], yValues[j] - yValues[j - 1] );

                painter->setBrush( brush( i - 1, j - 1, rect ) );

                double y1 = yMap.transform( yValues[j - 1] );
                double y2 = yMap.transform( yValues[j] );

                if ( doAlign )
                {
                    y1 = qRound( y1 );
                    y2 = qRound( y2 );
                }

                QwtPainter::drawRect( painter, x1, y1, x2 - x1, y2 - y1 );
            }
        }
    }
    else if ( m_orientations & Qt::Horizontal )
    {
        for ( int i = 1; i < xValues.size(); i++ )
        {
            const QRectF rect( xValues[i - 1], area.top(),
                xValues[i] - xValues[i - 1], area.bottom() );

            painter->setBrush( brush( i - 1, 0, rect ) );

            double x1 = xMap.transform( xValues[i - 1] );
            double x2 = xMap.transform( xValues[i] );

            if ( doAlign )
            {
                x1 = qRound( x1 );
                x2 = qRound( x2 );
            }

            QwtPainter::drawRect( painter,
                x1, canvasRect.top(), x2 - x1, canvasRect.height() );
        }
    }
    else if ( m_orientations & Qt::Vertical )
    {
        for ( int i = 1; i < yValues.size(); i++ )
        {
            const QRectF rect( area.left(), yValues[i - 1],
                area.width(), yValues[i] - yValues[i - 1] );

            painter->setBrush( brush( 0, i - 1, rect ) );

            double y1 = yMap.transform( yValues[i - 1] );
            double y2 = yMap.transform( yValues[i] );

            if ( doAlign )
            {
                y1 = qRound( y1 );
                y2 = qRound( y2 );
            }

            QwtPainter::drawRect( painter, canvasRect.left(), y1,
                                  canvasRect.width(), y2 - y1 );
        }
    }
}

const QwtScaleDiv &GridItem::xScaleDiv() const
{
    return m_xScaleDiv;
}

const QwtScaleDiv &GridItem::yScaleDiv() const
{
    return m_yScaleDiv;
}

void GridItem::updateScaleDiv( 
    const QwtScaleDiv& xScaleDiv, const QwtScaleDiv& yScaleDiv )
{
    if ( m_gridAttributes & AutoUpdate )
    {
        setXDiv( xScaleDiv );
        setYDiv( yScaleDiv );
    }
}

QBrush GridItem::brush( int row, int column, const QRectF & ) const
{
    /*
        We need some sort of origin to avoid, that the brush
        changes for the same rectangle when panning
     */
    if ( ( row + column ) % 2 )
        return QBrush( m_palette.brush( QPalette::Base ) );
    else
        return QBrush( m_palette.brush( QPalette::AlternateBase ) );
}
