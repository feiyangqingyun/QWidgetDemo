#include <qapplication.h>
#include <qtabwidget.h>
#include "compass_grid.h"
#include "cockpit_grid.h"

//-----------------------------------------------------------------
//
//      dials.cpp -- A demo program featuring QwtDial and friends
//
//-----------------------------------------------------------------

int main ( int argc, char **argv )
{
    QApplication a( argc, argv );

    QTabWidget tabWidget;
    tabWidget.addTab( new CompassGrid, "Compass" );
    tabWidget.addTab( new CockpitGrid, "Cockpit" );

    tabWidget.show();

    return a.exec();
}

