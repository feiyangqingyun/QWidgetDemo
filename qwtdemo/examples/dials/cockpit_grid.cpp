#include <qlayout.h>
#include <qtimer.h>
#include <qwt_analog_clock.h>
#include <qwt_round_scale_draw.h>
#include "attitude_indicator.h"
#include "speedo_meter.h"
#include "cockpit_grid.h"

CockpitGrid::CockpitGrid( QWidget *parent ):
    QFrame( parent )
{
    setAutoFillBackground( true );

    setPalette( colorTheme( QColor( Qt::darkGray ).dark( 150 ) ) );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 5 );
    layout->setMargin( 0 );

    int i;
    for ( i = 0; i < 3; i++ )
    {
        QwtDial *dial = createDial( i );
        layout->addWidget( dial, 0, i );
    }

    for ( i = 0; i < layout->columnCount(); i++ )
        layout->setColumnStretch( i, 1 );
}

QwtDial *CockpitGrid::createDial( int pos )
{
    QwtDial *dial = NULL;
    switch( pos )
    {
        case 0:
        {
            d_clock = new QwtAnalogClock( this );
#if 0
            // disable minor ticks
            d_clock->scaleDraw()->setTickLength( QwtScaleDiv::MinorTick, 0 );
#endif

            const QColor knobColor = QColor( Qt::gray ).light( 130 );

            for ( int i = 0; i < QwtAnalogClock::NHands; i++ )
            {
                QColor handColor = QColor( Qt::gray ).light( 150 );
                int width = 8;

                if ( i == QwtAnalogClock::SecondHand )
                {
                    handColor = Qt::gray;
                    width = 5;
                }

                QwtDialSimpleNeedle *hand = new QwtDialSimpleNeedle(
                    QwtDialSimpleNeedle::Arrow, true, handColor, knobColor );
                hand->setWidth( width );

                d_clock->setHand( static_cast<QwtAnalogClock::Hand>( i ), hand );
            }

            QTimer *timer = new QTimer( d_clock );
            timer->connect( timer, SIGNAL( timeout() ),
                d_clock, SLOT( setCurrentTime() ) );
            timer->start( 1000 );

            dial = d_clock;
            break;
        }
        case 1:
        {
            d_speedo = new SpeedoMeter( this );
            d_speedo->setScaleStepSize( 20.0 );
            d_speedo->setScale( 0.0, 240.0 );
            d_speedo->scaleDraw()->setPenWidth( 2 );

            QTimer *timer = new QTimer( d_speedo );
            timer->connect( timer, SIGNAL( timeout() ),
                this, SLOT( changeSpeed() ) );
            timer->start( 50 );

            dial = d_speedo;
            break;
        }
        case 2:
        {
            d_ai = new AttitudeIndicator( this );
            d_ai->scaleDraw()->setPenWidth( 3 );

            QTimer *gradientTimer = new QTimer( d_ai );
            gradientTimer->connect( gradientTimer, SIGNAL( timeout() ),
                this, SLOT( changeGradient() ) );
            gradientTimer->start( 100 );

            QTimer *angleTimer = new QTimer( d_ai );
            angleTimer->connect( angleTimer, SIGNAL( timeout() ),
                this, SLOT( changeAngle() ) );
            angleTimer->start( 100 );

            dial = d_ai;
            break;
        }

    }

    if ( dial )
    {
        dial->setReadOnly( true );
        dial->setLineWidth( 4 );
        dial->setFrameShadow( QwtDial::Sunken );
    }
    return dial;
}

QPalette CockpitGrid::colorTheme( const QColor &base ) const
{
    QPalette palette;
    palette.setColor( QPalette::Base, base );
    palette.setColor( QPalette::Window, base.dark( 150 ) );
    palette.setColor( QPalette::Mid, base.dark( 110 ) );
    palette.setColor( QPalette::Light, base.light( 170 ) );
    palette.setColor( QPalette::Dark, base.dark( 170 ) );
    palette.setColor( QPalette::Text, base.dark( 200 ).light( 800 ) );
    palette.setColor( QPalette::WindowText, base.dark( 200 ) );

    return palette;
}

void CockpitGrid::changeSpeed()
{
    static double offset = 0.8;

    double speed = d_speedo->value();

    if ( ( speed < 7.0 && offset < 0.0 ) ||
        ( speed > 203.0 && offset > 0.0 ) )
    {
        offset = -offset;
    }

    static int counter = 0;
    switch( counter++ % 12 )
    {
        case 0:
        case 2:
        case 7:
        case 8:
            break;
        default:
            d_speedo->setValue( speed + offset );
    }
}

void CockpitGrid::changeAngle()
{
    static double offset = 0.05;

    double angle = d_ai->angle();
    if ( angle > 180.0 )
        angle -= 360.0;

    if ( ( angle < -5.0 && offset < 0.0 ) ||
        ( angle > 5.0 && offset > 0.0 ) )
    {
        offset = -offset;
    }

    d_ai->setAngle( angle + offset );
}

void CockpitGrid::changeGradient()
{
    static double offset = 0.005;

    double gradient = d_ai->gradient();

    if ( ( gradient < -0.05 && offset < 0.0 ) ||
        ( gradient > 0.05 && offset > 0.0 ) )
    {
        offset = -offset;
    }

    d_ai->setGradient( gradient + offset );
}
