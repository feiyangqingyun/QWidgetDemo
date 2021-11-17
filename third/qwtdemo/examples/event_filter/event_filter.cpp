//-----------------------------------------------------------------
//      A demo program showing how to use event filtering
//-----------------------------------------------------------------

#include <qapplication.h>
#include <qmainwindow.h>
#include <qwhatsthis.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include "plot.h"
#include "canvaspicker.h"
#include "scalepicker.h"

int main ( int argc, char **argv )
{
    QApplication a( argc, argv );

    QMainWindow mainWindow;
    QToolBar *toolBar = new QToolBar( &mainWindow );
    QAction *action = QWhatsThis::createAction( toolBar );
    toolBar->addAction( action );
    mainWindow.addToolBar( toolBar );

    Plot *plot = new Plot( &mainWindow );

    // The canvas picker handles all mouse and key
    // events on the plot canvas

    ( void ) new CanvasPicker( plot );

    // The scale picker translates mouse clicks
    // int o clicked() signals

    ScalePicker *scalePicker = new ScalePicker( plot );
    a.connect( scalePicker, SIGNAL( clicked( int, double ) ),
        plot, SLOT( insertCurve( int, double ) ) );

    mainWindow.setCentralWidget( plot );

    mainWindow.resize( 540, 400 );
    mainWindow.show();

    const char *text =
        "An useless plot to demonstrate how to use event filtering.\n\n"
        "You can click on the color bar, the scales or move the wheel.\n"
        "All points can be moved using the mouse or the keyboard.";
    plot->setWhatsThis( text );

    int rv = a.exec();
    return rv;
}
