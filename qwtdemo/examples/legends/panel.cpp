#include "panel.h"
#include "settings.h"
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwt_plot.h>
#include <qwt_plot_legenditem.h>

Panel::Panel( QWidget *parent ):
    QWidget( parent )
{
    // create widgets

    d_legend.checkBox = new QCheckBox( "Enabled" );

    d_legend.positionBox = new QComboBox();
    d_legend.positionBox->addItem( "Left", QwtPlot::LeftLegend );
    d_legend.positionBox->addItem( "Right", QwtPlot::RightLegend );
    d_legend.positionBox->addItem( "Bottom", QwtPlot::BottomLegend );
    d_legend.positionBox->addItem( "Top", QwtPlot::TopLegend );
    d_legend.positionBox->addItem( "External", QwtPlot::TopLegend + 1 );

    d_legendItem.checkBox = new QCheckBox( "Enabled" );

    d_legendItem.numColumnsBox = new QSpinBox();
    d_legendItem.numColumnsBox->setRange( 0, 10 );
    d_legendItem.numColumnsBox->setSpecialValueText( "Unlimited" );

    d_legendItem.hAlignmentBox = new QComboBox();
    d_legendItem.hAlignmentBox->addItem( "Left", Qt::AlignLeft );
    d_legendItem.hAlignmentBox->addItem( "Centered", Qt::AlignHCenter );
    d_legendItem.hAlignmentBox->addItem( "Right", Qt::AlignRight );
    
    d_legendItem.vAlignmentBox = new QComboBox();
    d_legendItem.vAlignmentBox->addItem( "Top", Qt::AlignTop );
    d_legendItem.vAlignmentBox->addItem( "Centered", Qt::AlignVCenter );
    d_legendItem.vAlignmentBox->addItem( "Bottom", Qt::AlignBottom );

    d_legendItem.backgroundBox = new QComboBox();
    d_legendItem.backgroundBox->addItem( "Legend",
        QwtPlotLegendItem::LegendBackground );
    d_legendItem.backgroundBox->addItem( "Items",
        QwtPlotLegendItem::ItemBackground );

    d_legendItem.sizeBox = new QSpinBox();
    d_legendItem.sizeBox->setRange( 8, 22 );

    d_curve.numCurves = new QSpinBox();
    d_curve.numCurves->setRange( 0, 99 );

    d_curve.title = new QLineEdit();

    // layout

    QGroupBox *legendBox = new QGroupBox( "Legend" );
    QGridLayout *legendBoxLayout = new QGridLayout( legendBox );

    int row = 0;
    legendBoxLayout->addWidget( d_legend.checkBox, row, 0, 1, -1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Position" ), row, 0 );
    legendBoxLayout->addWidget( d_legend.positionBox, row, 1 );


    QGroupBox *legendItemBox = new QGroupBox( "Legend Item" );
    QGridLayout *legendItemBoxLayout = new QGridLayout( legendItemBox );

    row = 0;
    legendItemBoxLayout->addWidget( d_legendItem.checkBox, row, 0, 1, -1 );

    row++;
    legendItemBoxLayout->addWidget( new QLabel( "Columns" ), row, 0 );
    legendItemBoxLayout->addWidget( d_legendItem.numColumnsBox, row, 1 );

    row++;
    legendItemBoxLayout->addWidget( new QLabel( "Horizontal" ), row, 0 );
    legendItemBoxLayout->addWidget( d_legendItem.hAlignmentBox, row, 1 );

    row++;
    legendItemBoxLayout->addWidget( new QLabel( "Vertical" ), row, 0 );
    legendItemBoxLayout->addWidget( d_legendItem.vAlignmentBox, row, 1 );

    row++;
    legendItemBoxLayout->addWidget( new QLabel( "Background" ), row, 0 );
    legendItemBoxLayout->addWidget( d_legendItem.backgroundBox, row, 1 );

    row++;
    legendItemBoxLayout->addWidget( new QLabel( "Size" ), row, 0 );
    legendItemBoxLayout->addWidget( d_legendItem.sizeBox, row, 1 );

    QGroupBox *curveBox = new QGroupBox( "Curves" );
    QGridLayout *curveBoxLayout = new QGridLayout( curveBox );

    row = 0;
    curveBoxLayout->addWidget( new QLabel( "Number" ), row, 0 );
    curveBoxLayout->addWidget( d_curve.numCurves, row, 1 );

    row++;
    curveBoxLayout->addWidget( new QLabel( "Title" ), row, 0 );
    curveBoxLayout->addWidget( d_curve.title, row, 1 );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( legendBox );
    layout->addWidget( legendItemBox );
    layout->addWidget( curveBox );
    layout->addStretch( 10 );

    connect( d_legend.checkBox, 
        SIGNAL( stateChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legend.positionBox, 
        SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );

    connect( d_legendItem.checkBox, 
        SIGNAL( stateChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legendItem.numColumnsBox, 
        SIGNAL( valueChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legendItem.hAlignmentBox, 
        SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legendItem.vAlignmentBox, 
        SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legendItem.backgroundBox, 
        SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
    connect( d_curve.numCurves, 
        SIGNAL( valueChanged( int ) ), SIGNAL( edited() ) );
    connect( d_legendItem.sizeBox, 
        SIGNAL( valueChanged( int ) ), SIGNAL( edited() ) );
    connect( d_curve.title, 
        SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );
}

void Panel::setSettings( const Settings &settings)
{
    blockSignals( true );

    d_legend.checkBox->setCheckState(
        settings.legend.isEnabled ? Qt::Checked : Qt::Unchecked );
    d_legend.positionBox->setCurrentIndex( settings.legend.position );

    d_legendItem.checkBox->setCheckState(
        settings.legendItem.isEnabled ? Qt::Checked : Qt::Unchecked );

    d_legendItem.numColumnsBox->setValue( settings.legendItem.numColumns );

    int align = settings.legendItem.alignment;

    if ( align & Qt::AlignLeft )
        d_legendItem.hAlignmentBox->setCurrentIndex( 0 );
    else if ( align & Qt::AlignRight )
        d_legendItem.hAlignmentBox->setCurrentIndex( 2 );
    else
        d_legendItem.hAlignmentBox->setCurrentIndex( 1 );

    if ( align & Qt::AlignTop )
        d_legendItem.vAlignmentBox->setCurrentIndex( 0 );
    else if ( align & Qt::AlignBottom )
        d_legendItem.vAlignmentBox->setCurrentIndex( 2 );
    else
        d_legendItem.vAlignmentBox->setCurrentIndex( 1 );

    d_legendItem.backgroundBox->setCurrentIndex( 
        settings.legendItem.backgroundMode );

    d_legendItem.sizeBox->setValue( settings.legendItem.size );

    d_curve.numCurves->setValue( settings.curve.numCurves );
    d_curve.title->setText( settings.curve.title );

    blockSignals( false );
}

Settings Panel::settings() const
{
    Settings s;

    s.legend.isEnabled = 
        d_legend.checkBox->checkState() == Qt::Checked;
    s.legend.position = d_legend.positionBox->currentIndex();

    s.legendItem.isEnabled = 
        d_legendItem.checkBox->checkState() == Qt::Checked;
    s.legendItem.numColumns = d_legendItem.numColumnsBox->value();
    
    int align = 0;

    int hIndex = d_legendItem.hAlignmentBox->currentIndex();
    if ( hIndex == 0 )
        align |= Qt::AlignLeft;
    else if ( hIndex == 2 )
        align |= Qt::AlignRight;
    else
        align |= Qt::AlignHCenter;

    int vIndex = d_legendItem.vAlignmentBox->currentIndex();
    if ( vIndex == 0 )
        align |= Qt::AlignTop;
    else if ( vIndex == 2 )
        align |= Qt::AlignBottom;
    else
        align |= Qt::AlignVCenter;

    s.legendItem.alignment = align;

    s.legendItem.backgroundMode = 
            d_legendItem.backgroundBox->currentIndex();
    s.legendItem.size = d_legendItem.sizeBox->value();

    s.curve.numCurves = d_curve.numCurves->value();
    s.curve.title = d_curve.title->text();

    return s;
}
