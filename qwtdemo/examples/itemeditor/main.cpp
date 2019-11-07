#include "plot.h"
#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>

class MainWindow: public QMainWindow
{
public:
    MainWindow( QWidget * = NULL );
};

MainWindow::MainWindow( QWidget *parent ):
    QMainWindow( parent )
{
    Plot *plot = new Plot( this );
    setCentralWidget( plot );

    QToolBar *toolBar = new QToolBar( this );

    QComboBox *modeBox = new QComboBox( toolBar );
    modeBox->addItem( "No Mask" );
    modeBox->addItem( "Mask" );
    modeBox->addItem( "Alpha Mask" );
    modeBox->addItem( "Alpha Mask/Redraw" );
    modeBox->addItem( "Alpha Mask/Copy Mask" );
    modeBox->setCurrentIndex( 1 );
    modeBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( modeBox, SIGNAL( currentIndexChanged( int ) ),
             plot, SLOT( setMode( int ) ) );

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    connect( btnExport, SIGNAL( clicked() ), plot, SLOT( exportPlot() ) );

    toolBar->addWidget( modeBox );
    toolBar->addWidget( btnExport );
    addToolBar( toolBar );

}

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    MainWindow window;
    window.resize( 600, 400 );
    window.show();

    return app.exec();
}
