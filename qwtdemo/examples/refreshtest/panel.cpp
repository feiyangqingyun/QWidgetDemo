#include "panel.h"
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qwt_plot_curve.h>

class SpinBox: public QSpinBox
{
public:
    SpinBox( int min, int max, int step, QWidget *parent ):
        QSpinBox( parent )
    {
        setRange( min, max );
        setSingleStep( step );
    }
};

class CheckBox: public QCheckBox
{
public:
    CheckBox( const QString &title, QWidget *parent ):
        QCheckBox( title, parent )
    {
    }

    void setChecked( bool checked )
    {
        setCheckState( checked ? Qt::Checked : Qt::Unchecked );
    }

    bool isChecked() const
    {
        return checkState() == Qt::Checked;
    }
};

Panel::Panel( QWidget *parent ):
    QTabWidget( parent )
{
    setTabPosition( QTabWidget::West );

    addTab( createPlotTab( this ), "Plot" );
    addTab( createCanvasTab( this ), "Canvas" );
    addTab( createCurveTab( this ), "Curve" );

    setSettings( Settings() );

    connect( d_numPoints, SIGNAL( valueChanged( int ) ), SLOT( edited() ) );
    connect( d_updateInterval, SIGNAL( valueChanged( int ) ), SLOT( edited() ) );
    connect( d_curveWidth, SIGNAL( valueChanged( int ) ), SLOT( edited() ) );

    connect( d_paintCache, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_paintOnScreen, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_immediatePaint, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
#ifndef QWT_NO_OPENGL
    connect( d_openGL, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
#endif

    connect( d_curveAntialiasing, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_curveClipping, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_curveFiltering, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_lineSplitting, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );
    connect( d_curveFilled, SIGNAL( stateChanged( int ) ), SLOT( edited() ) );

    connect( d_updateType, SIGNAL( currentIndexChanged( int ) ), SLOT( edited() ) );
    connect( d_gridStyle, SIGNAL( currentIndexChanged( int ) ), SLOT( edited() ) );
    connect( d_curveType, SIGNAL( currentIndexChanged( int ) ), SLOT( edited() ) );
    connect( d_curvePen, SIGNAL( currentIndexChanged( int ) ), SLOT( edited() ) );
}

QWidget *Panel::createPlotTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );

    d_updateInterval = new SpinBox( 0, 1000, 10, page );
    d_numPoints = new SpinBox( 10, 1000000, 1000, page );

    d_updateType = new QComboBox( page );
    d_updateType->addItem( "Repaint" );
    d_updateType->addItem( "Replot" );

    int row = 0;

    QGridLayout *layout = new QGridLayout( page );

    layout->addWidget( new QLabel( "Updates", page ), row, 0 );
    layout->addWidget( d_updateInterval, row, 1 );
    layout->addWidget( new QLabel( "ms", page ), row++, 2 );

    layout->addWidget( new QLabel( "Points", page ), row, 0 );
    layout->addWidget( d_numPoints, row++, 1 );

    layout->addWidget( new QLabel( "Update", page ), row, 0 );
    layout->addWidget( d_updateType, row++, 1 );

    layout->addLayout( new QHBoxLayout(), row++, 0 );

    layout->setColumnStretch( 1, 10 );
    layout->setRowStretch( row, 10 );

    return page;
}

QWidget *Panel::createCanvasTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );

    d_gridStyle = new QComboBox( page );
    d_gridStyle->addItem( "None" );
    d_gridStyle->addItem( "Solid" );
    d_gridStyle->addItem( "Dashes" );

    d_paintCache = new CheckBox( "Paint Cache", page );
    d_paintOnScreen = new CheckBox( "Paint On Screen", page );
    d_immediatePaint = new CheckBox( "Immediate Paint", page );
#ifndef QWT_NO_OPENGL
    d_openGL = new CheckBox( "OpenGL", page );
#endif

    int row = 0;

    QGridLayout *layout = new QGridLayout( page );
    layout->addWidget( new QLabel( "Grid", page ), row, 0 );
    layout->addWidget( d_gridStyle, row++, 1 );

    layout->addWidget( d_paintCache, row++, 0, 1, -1 );
    layout->addWidget( d_paintOnScreen, row++, 0, 1, -1 );
    layout->addWidget( d_immediatePaint, row++, 0, 1, -1 );
#ifndef QWT_NO_OPENGL
    layout->addWidget( d_openGL, row++, 0, 1, -1 );
#endif

    layout->addLayout( new QHBoxLayout(), row++, 0 );

    layout->setColumnStretch( 1, 10 );
    layout->setRowStretch( row, 10 );

    return page;
}

QWidget *Panel::createCurveTab( QWidget *parent )
{
    QWidget *page = new QWidget( parent );

    d_curveType = new QComboBox( page );
    d_curveType->addItem( "Wave" );
    d_curveType->addItem( "Noise" );

    d_curveAntialiasing = new CheckBox( "Antialiasing", page );
    d_curveClipping = new CheckBox( "Clipping", page );
    d_curveFiltering = new CheckBox( "Filtering", page );
    d_lineSplitting = new CheckBox( "Split Lines", page );

    d_curveWidth = new SpinBox( 0, 10, 1, page );

    d_curvePen = new QComboBox( page );
    d_curvePen->addItem( "Solid" );
    d_curvePen->addItem( "Dotted" );

    d_curveFilled = new CheckBox( "Filled", page );

    int row = 0;

    QGridLayout *layout = new QGridLayout( page );
    layout->addWidget( new QLabel( "Type", page ), row, 0 );
    layout->addWidget( d_curveType, row++, 1 );

    layout->addWidget( d_curveAntialiasing, row++, 0, 1, -1 );
    layout->addWidget( d_curveClipping, row++, 0, 1, -1 );
    layout->addWidget( d_curveFiltering, row++, 0, 1, -1 );
    layout->addWidget( d_lineSplitting, row++, 0, 1, -1 );

    layout->addWidget( new QLabel( "Width", page ), row, 0 );
    layout->addWidget( d_curveWidth, row++, 1 );

    layout->addWidget( new QLabel( "Style", page ), row, 0 );
    layout->addWidget( d_curvePen, row++, 1 );

    layout->addWidget( d_curveFilled, row++, 0, 1, -1 );

    layout->addLayout( new QHBoxLayout(), row++, 0 );

    layout->setColumnStretch( 1, 10 );
    layout->setRowStretch( row, 10 );

    return page;
}

void Panel::edited()
{
    const Settings s = settings();
    Q_EMIT settingsChanged( s );
}


Settings Panel::settings() const
{
    Settings s;

    s.grid.pen = QPen( Qt::black, 0 );

    switch( d_gridStyle->currentIndex() )
    {
        case 0:
            s.grid.pen.setStyle( Qt::NoPen );
            break;
        case 2:
            s.grid.pen.setStyle( Qt::DashLine );
            break;
    }

    s.curve.pen.setStyle( d_curvePen->currentIndex() == 0 ?
        Qt::SolidLine : Qt::DotLine );
    s.curve.pen.setWidth( d_curveWidth->value() );
    s.curve.brush.setStyle( ( d_curveFilled->isChecked() ) ?
        Qt::SolidPattern : Qt::NoBrush );
    s.curve.numPoints = d_numPoints->value();
    s.curve.functionType = static_cast<Settings::FunctionType>(
        d_curveType->currentIndex() );
    if ( d_curveClipping->isChecked() )
        s.curve.paintAttributes |= QwtPlotCurve::ClipPolygons;
    else
        s.curve.paintAttributes &= ~QwtPlotCurve::ClipPolygons;
    if ( d_curveFiltering->isChecked() )
        s.curve.paintAttributes |= QwtPlotCurve::FilterPoints;
    else
        s.curve.paintAttributes &= ~QwtPlotCurve::FilterPoints;

    if ( d_curveAntialiasing->isChecked() )
        s.curve.renderHint |= QwtPlotItem::RenderAntialiased;
    else
        s.curve.renderHint &= ~QwtPlotItem::RenderAntialiased;

    s.curve.lineSplitting = ( d_lineSplitting->isChecked() );

    s.canvas.useBackingStore = ( d_paintCache->isChecked() );
    s.canvas.paintOnScreen = ( d_paintOnScreen->isChecked() );
    s.canvas.immediatePaint = ( d_immediatePaint->isChecked() );
#ifndef QWT_NO_OPENGL
    s.canvas.openGL = ( d_openGL->isChecked() );
#endif

    s.updateInterval = d_updateInterval->value();
    s.updateType = static_cast<Settings::UpdateType>( d_updateType->currentIndex() );

    return s;
}

void Panel::setSettings( const Settings &s )
{
    d_numPoints->setValue( s.curve.numPoints );
    d_updateInterval->setValue( s.updateInterval );
    d_updateType->setCurrentIndex( s.updateType );

    switch( s.grid.pen.style() )
    {
        case Qt::NoPen:
        {
            d_gridStyle->setCurrentIndex( 0 );
            break;
        }
        case Qt::DashLine:
        {
            d_gridStyle->setCurrentIndex( 2 );
            break;
        }
        default:
        {
            d_gridStyle->setCurrentIndex( 1 ); // Solid
        }
    }

    d_paintCache->setChecked( s.canvas.useBackingStore );
    d_paintOnScreen->setChecked( s.canvas.paintOnScreen );
    d_immediatePaint->setChecked( s.canvas.immediatePaint );
#ifndef QWT_NO_OPENGL
    d_openGL->setChecked( s.canvas.openGL );
#endif

    d_curveType->setCurrentIndex( s.curve.functionType );
    d_curveAntialiasing->setChecked(
        s.curve.renderHint & QwtPlotCurve::RenderAntialiased );

    d_curveClipping->setChecked(
        s.curve.paintAttributes & QwtPlotCurve::ClipPolygons );
    d_curveFiltering->setChecked(
        s.curve.paintAttributes & QwtPlotCurve::FilterPoints );

    d_lineSplitting->setChecked( s.curve.lineSplitting );

    d_curveWidth->setValue( s.curve.pen.width() );
    d_curvePen->setCurrentIndex(
        s.curve.pen.style() == Qt::SolidLine ? 0 : 1 );
    d_curveFilled->setChecked( s.curve.brush.style() != Qt::NoBrush );
}
