#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include "barchart.h"

class MainWindow: public QMainWindow
{
public:
    MainWindow( QWidget * = NULL );

private:
    BarChart *d_chart;
};

MainWindow::MainWindow( QWidget *parent ):
    QMainWindow( parent )
{
    d_chart = new BarChart( this );
    setCentralWidget( d_chart );

    QToolBar *toolBar = new QToolBar( this );

    QComboBox *orientationBox = new QComboBox( toolBar );
    orientationBox->addItem( "Vertical" );
    orientationBox->addItem( "Horizontal" );
    orientationBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    connect( btnExport, SIGNAL( clicked() ), d_chart, SLOT( exportChart() ) );

    toolBar->addWidget( orientationBox );
    toolBar->addWidget( btnExport );
    addToolBar( toolBar );

    d_chart->setOrientation( orientationBox->currentIndex() );
    connect( orientationBox, SIGNAL( currentIndexChanged( int ) ),
             d_chart, SLOT( setOrientation( int ) ) );
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MainWindow mainWindow;

    mainWindow.resize( 600, 400 );
    mainWindow.show();

    return a.exec();
}
