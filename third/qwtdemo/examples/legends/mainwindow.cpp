#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include "plot.h"
#include "panel.h"
#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent ):
    QMainWindow( parent )
{
    d_plot = new Plot();

    Settings settings;
    settings.legend.isEnabled = true;
    settings.legend.position = QwtPlot::BottomLegend;

    settings.legendItem.isEnabled = false;
    settings.legendItem.numColumns = 1;
    settings.legendItem.alignment = Qt::AlignRight | Qt::AlignVCenter;
    settings.legendItem.backgroundMode = 0;
    settings.legendItem.size = d_plot->canvas()->font().pointSize();

    settings.curve.numCurves = 4;
    settings.curve.title = "Curve";
    
    d_panel = new Panel();
    d_panel->setSettings( settings );

    QWidget *box = new QWidget( this );
    QHBoxLayout *layout = new QHBoxLayout( box );
    layout->addWidget( d_plot, 10 );
    layout->addWidget( d_panel );

    setCentralWidget( box );

    QToolBar *toolBar = new QToolBar( this );

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    toolBar->addWidget( btnExport );

    addToolBar( toolBar );

    updatePlot();

    connect( d_panel, SIGNAL( edited() ), SLOT( updatePlot() ) );
    connect( btnExport, SIGNAL( clicked() ), SLOT( exportPlot() ) );
}

void MainWindow::updatePlot()
{
    d_plot->applySettings( d_panel->settings() );
}

void MainWindow::exportPlot()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( d_plot, "legends.pdf" );
}
