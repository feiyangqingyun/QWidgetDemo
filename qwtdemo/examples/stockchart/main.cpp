#include <qapplication.h>
#include <qmainwindow.h>
#include <qcombobox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include "plot.h"

class MainWindow: public QMainWindow
{
public:
    MainWindow( QWidget * = NULL );

private:
    Plot *d_plot;
};

MainWindow::MainWindow( QWidget *parent ):
    QMainWindow( parent )
{
    d_plot = new Plot( this );
    setCentralWidget( d_plot );

    QToolBar *toolBar = new QToolBar( this );

    QComboBox *typeBox = new QComboBox( toolBar );
    typeBox->addItem( "Bars" );
    typeBox->addItem( "CandleSticks" );
    typeBox->setCurrentIndex( 1 );
    typeBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    connect( btnExport, SIGNAL( clicked() ), d_plot, SLOT( exportPlot() ) );

    toolBar->addWidget( typeBox );
    toolBar->addWidget( btnExport );
    addToolBar( toolBar );

    d_plot->setMode( typeBox->currentIndex() );
    connect( typeBox, SIGNAL( currentIndexChanged( int ) ),
        d_plot, SLOT( setMode( int ) ) );
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MainWindow w;
    w.resize( 600, 400 );
    w.show();

    return a.exec();
}
