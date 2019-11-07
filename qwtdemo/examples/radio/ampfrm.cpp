#include "ampfrm.h"
#include <qwt_knob.h>
#include <qwt_thermo.h>
#include <qwt_round_scale_draw.h>
#include <qwt_math.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qfont.h>
#include <qpen.h>
#include <qevent.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#define qFastCos(x) ::cos(x)
#endif

class Knob: public QWidget
{
public:
    Knob( const QString &title, double min, double max, QWidget *parent ):
        QWidget( parent )
    {
        d_knob = new QwtKnob( this );
        d_knob->setScale( min, max );
        d_knob->setTotalSteps( 0 ); // disable
        d_knob->setScaleMaxMajor( 10 );

        d_knob->setKnobStyle( QwtKnob::Raised );
        d_knob->setKnobWidth( 50 );
        d_knob->setBorderWidth( 2 );
        d_knob->setMarkerStyle( QwtKnob::Notch );
        d_knob->setMarkerSize( 8 );

        d_knob->scaleDraw()->setTickLength( QwtScaleDiv::MinorTick, 4 );
        d_knob->scaleDraw()->setTickLength( QwtScaleDiv::MediumTick, 4 );
        d_knob->scaleDraw()->setTickLength( QwtScaleDiv::MajorTick, 6 );

        d_label = new QLabel( title, this );
        d_label->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

        setSizePolicy( QSizePolicy::MinimumExpanding,
            QSizePolicy::MinimumExpanding );
    }

    virtual QSize sizeHint() const
    {
        QSize sz1 = d_knob->sizeHint();
        QSize sz2 = d_label->sizeHint();

        const int w = qMax( sz1.width(), sz2.width() );
        const int h = sz1.height() + sz2.height();

        int off = qCeil( d_knob->scaleDraw()->extent( d_knob->font() ) );
        off -= 10; // spacing

        return QSize( w, h - off );
    }

    void setValue( double value )
    {
        d_knob->setValue( value );
    }

    double value() const
    {
        return d_knob->value();
    }

protected:
    virtual void resizeEvent( QResizeEvent *e )
    {
        const QSize sz = e->size();

        int h = d_label->sizeHint().height();

        d_label->setGeometry( 0, sz.height() - h, sz.width(), h );

        h = d_knob->sizeHint().height();
        int off = qCeil( d_knob->scaleDraw()->extent( d_knob->font() ) );
        off -= 10; // spacing

        d_knob->setGeometry( 0, d_label->pos().y() - h + off,
            sz.width(), h );
    }

private:
    QwtKnob *d_knob;
    QLabel *d_label;
};

class Thermo: public QWidget
{
public:
    Thermo( const QString &title, QWidget *parent ):
        QWidget( parent )
    {
        d_thermo = new QwtThermo( this );
        d_thermo->setPipeWidth( 6 );
        d_thermo->setScale( -40, 10 );
        d_thermo->setFillBrush( Qt::green );
        d_thermo->setAlarmBrush( Qt::red );
        d_thermo->setAlarmLevel( 0.0 );
        d_thermo->setAlarmEnabled( true );

        QLabel *label = new QLabel( title, this );
        label->setAlignment( Qt::AlignTop | Qt::AlignLeft );

        QVBoxLayout *layout = new QVBoxLayout( this );
        layout->setMargin( 0 );
        layout->setSpacing( 0 );
        layout->addWidget( d_thermo, 10 );
        layout->addWidget( label );
    }

    void setValue( double value )
    {
        d_thermo->setValue( value );
    }

private:
    QwtThermo *d_thermo;
};

AmpFrame::AmpFrame( QWidget *p ):
    QFrame( p )
{
    d_knbVolume = new Knob( "Volume", 0.0, 10.0, this );
    d_knbBalance = new Knob( "Balance", -10.0, 10.0, this );
    d_knbTreble = new Knob( "Treble", -10.0, 10.0, this );
    d_knbBass = new Knob( "Bass", -10.0, 10.0, this );

    d_thmLeft = new Thermo( "Left [dB]", this );
    d_thmRight = new Thermo( "Right [dB]", this );

    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 10 );
    layout->addWidget( d_knbVolume );
    layout->addWidget( d_knbBalance);
    layout->addWidget( d_knbTreble);
    layout->addWidget( d_knbBass );
    layout->addSpacing( 20 );
    layout->addStretch( 10 );
    layout->addWidget( d_thmLeft );
    layout->addSpacing( 10 );
    layout->addWidget( d_thmRight );

    d_knbVolume->setValue( 7.0 );
    ( void )startTimer( 50 );
}

void AmpFrame::timerEvent( QTimerEvent * )
{
    static double phs = 0;

    //
    //  This amplifier generates its own input signal...
    //

    const double sig_bass = ( 1.0 + 0.1 * d_knbBass->value() )
        * qFastSin( 13.0 * phs );
    const double sig_mid_l = qFastSin( 17.0 * phs );
    const double sig_mid_r = qFastCos( 17.5 * phs );
    const double sig_trbl_l = 0.5 * ( 1.0 + 0.1 * d_knbTreble->value() )
        * qFastSin( 35.0 * phs );
    const double sig_trbl_r = 0.5 * ( 1.0 + 0.1 * d_knbTreble->value() )
        * qFastSin( 34.0 * phs );

    double sig_l = 0.05 * d_master * d_knbVolume->value()
        * qwtSqr( sig_bass + sig_mid_l + sig_trbl_l );
    double sig_r = 0.05 * d_master * d_knbVolume->value()
        * qwtSqr( sig_bass + sig_mid_r + sig_trbl_r );

    double balance = 0.1 * d_knbBalance->value();
    if ( balance > 0 )
        sig_l *= ( 1.0 - balance );
    else
        sig_r *= ( 1.0 + balance );

    if ( sig_l > 0.01 )
        sig_l = 20.0 * log10( sig_l );
    else
        sig_l = -40.0;

    if ( sig_r > 0.01 )
        sig_r = 20.0 * log10( sig_r );
    else
        sig_r = - 40.0;

    d_thmLeft->setValue( sig_l );
    d_thmRight->setValue( sig_r );

    phs += M_PI / 100;
    if ( phs > M_PI )
        phs = 0;
}

void AmpFrame::setMaster( double v )
{
    d_master = v;
}
