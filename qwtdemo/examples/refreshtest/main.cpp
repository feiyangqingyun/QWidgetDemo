#include "mainwindow.h"
#include <qapplication.h>

#ifndef QWT_NO_OPENGL
#if QT_VERSION >= 0x040600 && QT_VERSION < 0x050000
#define USE_OPENGL 1
#endif
#endif

#if USE_OPENGL
#include <qgl.h>
#endif

int main( int argc, char **argv )
{
#if USE_OPENGL
    // on my box QPaintEngine::OpenGL2 has serious problems, f.e:
    // the lines of a simple drawRect are wrong.

    QGL::setPreferredPaintEngine( QPaintEngine::OpenGL );
#endif

    QApplication a( argc, argv );

    MainWindow mainWindow;
    mainWindow.resize( 600, 400 );
    mainWindow.show();

    return a.exec();
}
