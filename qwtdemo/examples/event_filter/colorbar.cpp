#include <qevent.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include "colorbar.h"

ColorBar::ColorBar( Qt::Orientation o, QWidget *parent ):
    QWidget( parent ),
    d_orientation( o ),
    d_light( Qt::white ),
    d_dark( Qt::black )
{
#ifndef QT_NO_CURSOR
    setCursor( Qt::PointingHandCursor );
#endif
}

void ColorBar::setOrientation( Qt::Orientation o )
{
    d_orientation = o;
    update();
}

void ColorBar::setLight( const QColor &light )
{
    d_light = light;
    update();
}

void ColorBar::setDark( const QColor &dark )
{
    d_dark = dark;
    update();
}

void ColorBar::setRange( const QColor &light, const QColor &dark )
{
    d_light = light;
    d_dark = dark;
    update();
}

void ColorBar::mousePressEvent( QMouseEvent *e )
{
    if( e->button() ==  Qt::LeftButton )
    {
        // emit the color of the position where the mouse click
        // happened

        const QPixmap pm = QPixmap::grabWidget( this );
        const QRgb rgb = pm.toImage().pixel( e->x(), e->y() );

        Q_EMIT selected( QColor( rgb ) );
        e->accept();
    }
}

void ColorBar::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    drawColorBar( &painter, rect() );
}

void ColorBar::drawColorBar( QPainter *painter, const QRect &rect ) const
{
    int h1, s1, v1;
    int h2, s2, v2;

    d_light.getHsv( &h1, &s1, &v1 );
    d_dark.getHsv( &h2, &s2, &v2 );

    painter->save();
    painter->setClipRect( rect );
    painter->setClipping( true );

    painter->fillRect( rect, d_dark );

    const int sectionSize = 2;

    int numIntervals;
    if ( d_orientation == Qt::Horizontal )
        numIntervals = rect.width() / sectionSize;
    else
        numIntervals = rect.height() / sectionSize;

    for ( int i = 0; i < numIntervals; i++ )
    {
        QRect section;
        if ( d_orientation == Qt::Horizontal )
        {
            section.setRect( rect.x() + i * sectionSize, rect.y(),
                sectionSize, rect.height() );
        }
        else
        {
            section.setRect( rect.x(), rect.y() + i * sectionSize,
                rect.width(), sectionSize );
        }

        const double ratio = i / static_cast<double>( numIntervals );

        QColor c;
        c.setHsv( h1 + qRound( ratio * ( h2 - h1 ) ),
            s1 + qRound( ratio * ( s2 - s1 ) ),
            v1 + qRound( ratio * ( v2 - v1 ) ) );

        painter->fillRect( section, c );
    }

    painter->restore();
}

