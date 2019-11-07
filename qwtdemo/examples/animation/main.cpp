#include <qapplication.h>
#include "plot.h"

#ifndef QWT_NO_OPENGL
#define USE_OPENGL 1
#endif

#if USE_OPENGL
#include <qgl.h>
#include <qwt_plot_glcanvas.h>
#else
#include <qwt_plot_canvas.h>
#endif

int main ( int argc, char **argv )
{
#if USE_OPENGL
#if QT_VERSION >= 0x040600 && QT_VERSION < 0x050000
    // on my box QPaintEngine::OpenGL2 has serious problems, f.e:
    // the lines of a simple drawRect are wrong.

    QGL::setPreferredPaintEngine( QPaintEngine::OpenGL );
#endif
#endif

    QApplication a( argc, argv );

    Plot plot;

#if USE_OPENGL
    QwtPlotGLCanvas *canvas = new QwtPlotGLCanvas();
    canvas->setFrameStyle( QwtPlotGLCanvas::NoFrame );
#else
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFrameStyle( QFrame::NoFrame );
    canvas->setPaintAttribute( QwtPlotCanvas::BackingStore, false );
#endif

    plot.setCanvas( canvas );
    plot.setCanvasBackground( QColor( 30, 30, 50 ) );

    plot.resize( 400, 400 );
    plot.show();

    return a.exec();
}
