#include <qlabel.h>
#include <qlayout.h>
#include <qwt_dial.h>
#include <qwt_dial_needle.h>
#include <qwt_scale_engine.h>
#include <qwt_transform.h>
#include <qwt_round_scale_draw.h>
#include "dialbox.h"

DialBox::DialBox( QWidget *parent, int type ):
    QWidget( parent )
{
    d_dial = createDial( type );

    d_label = new QLabel( this );
    d_label->setAlignment( Qt::AlignCenter );

    QVBoxLayout *layout = new QVBoxLayout( this );;
    layout->setSpacing( 0 );
    layout->addWidget( d_dial, 10 );
    layout->addWidget( d_label );

    connect( d_dial, SIGNAL( valueChanged( double ) ), 
        this, SLOT( setNum( double ) ) );

    setNum( d_dial->value() );
}

QwtDial *DialBox::createDial( int type ) const
{
    QwtDial *dial = new QwtDial();
    dial->setTracking( true );
    dial->setFocusPolicy( Qt::StrongFocus );
    dial->setObjectName( QString( "Dial %1" ).arg( type + 1 ) );

    QColor needleColor( Qt::red );

    switch( type )
    {
        case 0:
        {
            dial->setOrigin( 135.0 );
            dial->setScaleArc( 0.0, 270.0 );
            dial->setScaleMaxMinor( 4 );
            dial->setScaleMaxMajor( 10 );
            dial->setScale( -100.0, 100.0 );

            needleColor = QColor( "Goldenrod" );

            break;
        }
        case 1:
        {
            dial->setOrigin( 135.0 );
            dial->setScaleArc( 0.0, 270.0 );
            dial->setScaleMaxMinor( 10 );
            dial->setScaleMaxMajor( 10 );
            dial->setScale( 10.0, 0.0 );

            QwtRoundScaleDraw *scaleDraw = new QwtRoundScaleDraw();
            scaleDraw->setSpacing( 8 );
            scaleDraw->enableComponent( 
                QwtAbstractScaleDraw::Backbone, false );
            scaleDraw->setTickLength( QwtScaleDiv::MinorTick, 2 );
            scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 4 );
            scaleDraw->setTickLength( QwtScaleDiv::MajorTick, 8 );
            dial->setScaleDraw( scaleDraw );

            break;
        }
        case 2:
        {
            dial->setOrigin( 150.0 );
            dial->setScaleArc( 0.0, 240.0 );

            QwtLinearScaleEngine *scaleEngine = new QwtLinearScaleEngine( 2 );
            scaleEngine->setTransformation( new QwtPowerTransform( 2 ) );
            dial->setScaleEngine( scaleEngine );

            QList< double > ticks[ QwtScaleDiv::NTickTypes ];
            ticks[ QwtScaleDiv::MajorTick ] << 0 << 4 
                << 16 << 32 << 64 << 96 << 128;
            ticks[ QwtScaleDiv::MediumTick ] << 24 << 48 << 80 << 112;
            ticks[ QwtScaleDiv::MinorTick ] 
                << 0.5 << 1 << 2 
                << 7 << 10 << 13
                << 20 << 28 
                << 40 << 56 
                << 72 << 88 
                << 104 << 120; 
 
            dial->setScale( QwtScaleDiv( 0, 128, ticks ) );
            break;
        }
        case 3:
        {
            dial->setOrigin( 135.0 );
            dial->setScaleArc( 0.0, 270.0 );
            dial->setScaleMaxMinor( 9 );
            dial->setScaleEngine( new QwtLogScaleEngine );
            dial->setScale( 1.0e-2, 1.0e2 );

            break;
        }
        case 4:
        {
            dial->setOrigin( 225.0 );
            dial->setScaleArc( 0.0, 360.0 );
            dial->setScaleMaxMinor( 5 );
            dial->setScaleStepSize( 20 );
            dial->setScale( 100.0, -100.0 );
            dial->setWrapping( true );
            dial->setTotalSteps( 40 );
            dial->setMode( QwtDial::RotateScale );
            dial->setValue( 70.0 );

            needleColor = QColor( "DarkSlateBlue" );

            break;
        }
        case 5:
        {
            dial->setOrigin( 45.0 );
            dial->setScaleArc( 0.0, 225.0 );
            dial->setScaleMaxMinor( 5 );
            dial->setScaleMaxMajor( 10 );
            dial->setScale( 0.0, 10.0 );

            break;
        }
    }

    QwtDialSimpleNeedle *needle = new QwtDialSimpleNeedle(
        QwtDialSimpleNeedle::Arrow, true, needleColor,
        QColor( Qt::gray ).light( 130 ) );
    dial->setNeedle( needle );

    //const QColor base( QColor( "DimGray" ) );
    const QColor base( QColor( Qt::darkGray ).dark( 150 ) );

    QPalette palette;
    palette.setColor( QPalette::Base, base );
    palette.setColor( QPalette::Window, base.dark( 150 ) );
    palette.setColor( QPalette::Mid, base.dark( 110 ) );
    palette.setColor( QPalette::Light, base.light( 170 ) );
    palette.setColor( QPalette::Dark, base.dark( 170 ) );
    palette.setColor( QPalette::Text, base.dark( 200 ).light( 800 ) );
    palette.setColor( QPalette::WindowText, base.dark( 200 ) );

    dial->setPalette( palette );
    dial->setLineWidth( 4 );
    dial->setFrameShadow( QwtDial::Sunken );

    return dial;
}

void DialBox::setNum( double v )
{
    QString text;
    text.setNum( v, 'f', 2 );

    d_label->setText( text );
}
