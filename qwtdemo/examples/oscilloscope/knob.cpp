#include "knob.h"
#include <qwt_math.h>
#include <qpen.h>
#include <qwt_knob.h>
#include <qwt_round_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qevent.h>

Knob::Knob( const QString &title, double min, double max, QWidget *parent ):
    QWidget( parent )
{
    QFont font( "Helvetica", 10 );

    d_knob = new QwtKnob( this );
    d_knob->setFont( font );

    QwtScaleDiv scaleDiv =
        d_knob->scaleEngine()->divideScale( min, max, 5, 3 );

    QList<double> ticks = scaleDiv.ticks( QwtScaleDiv::MajorTick );
    if ( ticks.size() > 0 && ticks[0] > min )
    {
        if ( ticks.first() > min )
            ticks.prepend( min );
        if ( ticks.last() < max )
            ticks.append( max );
    }
    scaleDiv.setTicks( QwtScaleDiv::MajorTick, ticks );
    d_knob->setScale( scaleDiv );

    d_knob->setKnobWidth( 50 );

    font.setBold( true );
    d_label = new QLabel( title, this );
    d_label->setFont( font );
    d_label->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

    setSizePolicy( QSizePolicy::MinimumExpanding,
        QSizePolicy::MinimumExpanding );

    connect( d_knob, SIGNAL( valueChanged( double ) ),
        this, SIGNAL( valueChanged( double ) ) );
}

QSize Knob::sizeHint() const
{
    QSize sz1 = d_knob->sizeHint();
    QSize sz2 = d_label->sizeHint();

    const int w = qMax( sz1.width(), sz2.width() );
    const int h = sz1.height() + sz2.height();

    int off = qCeil( d_knob->scaleDraw()->extent( d_knob->font() ) );
    off -= 15; // spacing

    return QSize( w, h - off );
}

void Knob::setValue( double value )
{
    d_knob->setValue( value );
}

double Knob::value() const
{
    return d_knob->value();
}

void Knob::setTheme( const QColor &color )
{
    d_knob->setPalette( color );
}

QColor Knob::theme() const
{
    return d_knob->palette().color( QPalette::Window );
}

void Knob::resizeEvent( QResizeEvent *event )
{
    const QSize sz = event->size();
    const QSize hint = d_label->sizeHint();

    d_label->setGeometry( 0, sz.height() - hint.height(),
        sz.width(), hint.height() );

    const int knobHeight = d_knob->sizeHint().height();

    int off = qCeil( d_knob->scaleDraw()->extent( d_knob->font() ) );
    off -= 15; // spacing

    d_knob->setGeometry( 0, d_label->pos().y() - knobHeight + off,
        sz.width(), knobHeight );
}
