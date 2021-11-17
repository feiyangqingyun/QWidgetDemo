#include <qlabel.h>
#include <qlayout.h>
#include <qwt_wheel.h>
#include <qwt_thermo.h>
#include <qwt_scale_engine.h>
#include <qwt_transform.h>
#include <qwt_color_map.h>
#include "wheelbox.h"

WheelBox::WheelBox( Qt::Orientation orientation,
        int type, QWidget *parent ):
    QWidget( parent )
{
    QWidget *box = createBox( orientation, type );
    d_label = new QLabel( this );
    d_label->setAlignment( Qt::AlignHCenter | Qt::AlignTop );

    QBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( box );
    layout->addWidget( d_label );

    setNum( d_wheel->value() );

    connect( d_wheel, SIGNAL( valueChanged( double ) ), 
        this, SLOT( setNum( double ) ) );
}

QWidget *WheelBox::createBox( 
    Qt::Orientation orientation, int type ) 
{
    d_wheel = new QwtWheel();
    d_wheel->setValue( 80 );
    d_wheel->setWheelWidth( 20 );
    d_wheel->setMass( 1.0 );

    d_thermo = new QwtThermo();
    d_thermo->setOrientation( orientation );

    if ( orientation == Qt::Horizontal )
    {
        d_thermo->setScalePosition( QwtThermo::LeadingScale );
        d_wheel->setOrientation( Qt::Vertical );
    }
    else
    {
        d_thermo->setScalePosition( QwtThermo::TrailingScale );
        d_wheel->setOrientation( Qt::Horizontal );
    }

    switch( type )
    {
        case 0:
        {
            QwtLinearColorMap *colorMap = new QwtLinearColorMap(); 
            colorMap->setColorInterval( Qt::blue, Qt::red );
            d_thermo->setColorMap( colorMap );

            break;
        }
        case 1:
        {
            QwtLinearColorMap *colorMap = new QwtLinearColorMap();
            colorMap->setMode( QwtLinearColorMap::FixedColors );

            int idx = 4;

            colorMap->setColorInterval( Qt::GlobalColor( idx ),
                Qt::GlobalColor( idx + 10 ) );
            for ( int i = 1; i < 10; i++ )
            {
                colorMap->addColorStop( i / 10.0, 
                    Qt::GlobalColor( idx + i ) );
            }

            d_thermo->setColorMap( colorMap );
            break;
        }
        case 2:
        {
            d_wheel->setRange( 10, 1000 );
            d_wheel->setSingleStep( 1.0 );

            d_thermo->setScaleEngine( new QwtLogScaleEngine );
            d_thermo->setScaleMaxMinor( 10 );

            d_thermo->setFillBrush( Qt::darkCyan );
            d_thermo->setAlarmBrush( Qt::magenta );
            d_thermo->setAlarmLevel( 500.0 );

            d_wheel->setValue( 800 );

            break;
        }
        case 3:
        {
            d_wheel->setRange( -1000, 1000 );
            d_wheel->setSingleStep( 1.0 );
            d_wheel->setPalette( QColor( "Tan" ) );

            QwtLinearScaleEngine *scaleEngine = new QwtLinearScaleEngine();
            scaleEngine->setTransformation( new QwtPowerTransform( 2 ) );

            d_thermo->setScaleMaxMinor( 5 );
            d_thermo->setScaleEngine( scaleEngine );

            QPalette pal = palette();
            pal.setColor( QPalette::Base, Qt::darkGray );
            pal.setColor( QPalette::ButtonText, QColor( "darkKhaki" ) );

            d_thermo->setPalette( pal );
            break;
        }
        case 4:
        {
            d_wheel->setRange( -100, 300 );
            d_wheel->setInverted( true );

            QwtLinearColorMap *colorMap = new QwtLinearColorMap(); 
            colorMap->setColorInterval( Qt::darkCyan, Qt::yellow );
            d_thermo->setColorMap( colorMap );

            d_wheel->setValue( 243 );

            break;
        }
        case 5:
        {
            d_thermo->setFillBrush( Qt::darkCyan );
            d_thermo->setAlarmBrush( Qt::magenta );
            d_thermo->setAlarmLevel( 60.0 );

            break;
        }
        case 6:
        {
            d_thermo->setOriginMode( QwtThermo::OriginMinimum );
            d_thermo->setFillBrush( QBrush( "DarkSlateBlue" ) );
            d_thermo->setAlarmBrush( QBrush( "DarkOrange" ) );
            d_thermo->setAlarmLevel( 60.0 );

            break;
        }
        case 7:
        {
            d_wheel->setRange( -100, 100 );

            d_thermo->setOriginMode( QwtThermo::OriginCustom );
            d_thermo->setOrigin( 0.0 );
            d_thermo->setFillBrush( Qt::darkBlue );

            break;
        }
    }

    double min = d_wheel->minimum();
    double max = d_wheel->maximum();

    if ( d_wheel->isInverted() )
        qSwap( min, max );

    d_thermo->setScale( min, max );
    d_thermo->setValue( d_wheel->value() );

    connect( d_wheel, SIGNAL( valueChanged( double ) ), 
        d_thermo, SLOT( setValue( double ) ) );

    QWidget *box = new QWidget();

    QBoxLayout *layout;

    if ( orientation == Qt::Horizontal )
        layout = new QHBoxLayout( box );
    else
        layout = new QVBoxLayout( box );

    layout->addWidget( d_thermo, Qt::AlignCenter );
    layout->addWidget( d_wheel );

    return box;
}

void WheelBox::setNum( double v )
{
    QString text;
    text.setNum( v, 'f', 2 );

    d_label->setText( text );
}
