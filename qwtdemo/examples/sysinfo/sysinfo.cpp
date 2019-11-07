#include <qapplication.h>
#include <qwidget.h>
#include <qfont.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qwt_thermo.h>
#include <qwt_color_map.h>

class ValueBar: public QWidget
{
public:
    ValueBar( Qt::Orientation orientation,
              const QString &text, QWidget *parent, double value = 0.0 ):
        QWidget( parent )
    {
        d_label = new QLabel( text, this );
        d_label->setFont( QFont( "Helvetica", 10 ) );

        d_thermo = new QwtThermo( this );
        d_thermo->setOrientation( orientation );
        d_thermo->setScale( 0.0, 100.0 );
        d_thermo->setValue( value );
        d_thermo->setFont( QFont( "Helvetica", 8 ) );
        d_thermo->setPipeWidth( 6 );
        d_thermo->setScaleMaxMajor( 6 );
        d_thermo->setScaleMaxMinor( 5 );
        d_thermo->setFillBrush( Qt::darkMagenta );

#if 0
        QwtLinearColorMap *colorMap =
            new QwtLinearColorMap( Qt::blue, Qt::red );

        colorMap->addColorStop( 0.2, Qt::yellow );
        colorMap->addColorStop( 0.3, Qt::cyan );
        colorMap->addColorStop( 0.4, Qt::green );
        colorMap->addColorStop( 0.5, Qt::magenta );
        colorMap->setMode( QwtLinearColorMap::FixedColors );
        d_thermo->setColorMap( colorMap );
#endif

        QVBoxLayout *layout = new QVBoxLayout( this );
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        if ( orientation == Qt::Horizontal )
        {
            d_label->setAlignment( Qt::AlignCenter );
            d_thermo->setScalePosition( QwtThermo::LeadingScale );
            layout->addWidget( d_label );
            layout->addWidget( d_thermo );
        }
        else
        {
            d_label->setAlignment( Qt::AlignRight );
            d_thermo->setScalePosition( QwtThermo::TrailingScale );
            layout->addWidget( d_thermo, 10, Qt::AlignHCenter );
            layout->addWidget( d_label, 0 );
        }
    }

    void setValue( double value )
    {
        d_thermo->setValue( value );
    }

private:
    QLabel *d_label;
    QwtThermo *d_thermo;
};

class SysInfo : public QFrame
{
public:
    SysInfo( QWidget *parent = NULL ):
        QFrame( parent )
    {
        QGroupBox *memBox = new QGroupBox( "Memory Usage", this );
        memBox->setFont( QFont( "Helvetica", 10 ) );

        QVBoxLayout *memLayout = new QVBoxLayout( memBox );
        memLayout->setMargin( 15 );
        memLayout->setSpacing( 5 );

        Qt::Orientation o = Qt::Horizontal;
        memLayout->addWidget( new ValueBar( o, "Used", memBox, 57 ) );
        memLayout->addWidget( new ValueBar( o, "Shared", memBox, 17 ) );
        memLayout->addWidget( new ValueBar( o, "Cache", memBox, 30 ) );
        memLayout->addWidget( new ValueBar( o, "Buffers", memBox, 22 ) );
        memLayout->addWidget( new ValueBar( o, "Swap Used", memBox, 57 ) );
        memLayout->addWidget( new QWidget( memBox ), 10 ); // spacer

        QGroupBox *cpuBox = new QGroupBox( "Cpu Usage", this );
        cpuBox->setFont( QFont( "Helvetica", 10 ) );

        QHBoxLayout *cpuLayout = new QHBoxLayout( cpuBox );
        cpuLayout->setMargin( 15 );
        cpuLayout->setSpacing( 5 );

        o = Qt::Vertical;
        cpuLayout->addWidget( new ValueBar( o, "User", cpuBox, 57 ) );
        cpuLayout->addWidget( new ValueBar( o, "Total", cpuBox, 73 ) );
        cpuLayout->addWidget( new ValueBar( o, "System", cpuBox, 16 ) );
        cpuLayout->addWidget( new ValueBar( o, "Idle", cpuBox, 27 ) );

        QHBoxLayout *layout = new QHBoxLayout( this );
        layout->setMargin( 10 );
        layout->addWidget( memBox, 10 );
        layout->addWidget( cpuBox, 0 );
    }
};

int main ( int argc, char **argv )
{
    QApplication a( argc, argv );

    SysInfo info;
    info.resize( info.sizeHint().expandedTo( QSize( 600, 400 ) ) );
    info.show();

    int rv = a.exec();
    return rv;
}
