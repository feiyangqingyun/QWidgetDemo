#include <qapplication.h>
#include "mainwindow.h"

int main ( int argc, char **argv )
{
    QApplication a( argc, argv );
    a.setStyle( "Windows" );

    MainWindow w;
    w.resize( 700, 500 );
    w.show();

    return a.exec();
}
