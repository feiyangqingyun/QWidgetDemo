
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qframe.h>

//------------------------------------------------------------
//      curvdemo1
//
//  This example program features some of the different
//  display styles of the QwtPlotCurve class
//------------------------------------------------------------


//
//   Array Sizes
//
const int Size = 27;
const int CurvCnt = 6;

//
//   Arrays holding the values
//
double xval[Size];
double yval[Size];
QwtScaleMap xMap;
QwtScaleMap yMap;

class MainWin : public QFrame
{
public:
    MainWin();

protected:
    virtual void paintEvent( QPaintEvent * );
    void drawContents( QPainter *p );

private:
    void shiftDown( QRect &rect, int offset ) const;

    QwtPlotCurve d_curves[CurvCnt];
};

MainWin::MainWin()
{
    int i;

    xMap.setScaleInterval( -0.5, 10.5 );
    yMap.setScaleInterval( -1.1, 1.1 );

    //
    //  Frame style
    //
    setFrameStyle( QFrame::Box | QFrame::Raised );
    setLineWidth( 2 );
    setMidLineWidth( 3 );

    //
    // Calculate values
    //
    for( i = 0; i < Size; i++ )
    {
        xval[i] = double( i ) * 10.0 / double( Size - 1 );
        yval[i] = qSin( xval[i] ) * qCos( 2.0 * xval[i] );
    }

    //
    //  define curve styles
    //
    i = 0;

    d_curves[i].setSymbol( new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
        QPen( Qt::black ), QSize( 5, 5 ) ) );
    d_curves[i].setPen( Qt::darkGreen );
    d_curves[i].setStyle( QwtPlotCurve::Lines );
    d_curves[i].setCurveAttribute( QwtPlotCurve::Fitted );
    i++;

    d_curves[i].setSymbol( new QwtSymbol( QwtSymbol::Ellipse, Qt::yellow,
        QPen( Qt::blue ), QSize( 5, 5 ) ) );
    d_curves[i].setPen( Qt::red );
    d_curves[i].setStyle( QwtPlotCurve::Sticks );
    i++;

    d_curves[i].setPen( Qt::darkBlue );
    d_curves[i].setStyle( QwtPlotCurve::Lines );
    i++;

    d_curves[i].setPen( Qt::darkBlue );
    d_curves[i].setStyle( QwtPlotCurve::Lines );
    d_curves[i].setRenderHint( QwtPlotItem::RenderAntialiased );
    i++;

    d_curves[i].setPen( Qt::darkCyan );
    d_curves[i].setStyle( QwtPlotCurve::Steps );
    i++;

    d_curves[i].setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::NoBrush,
        QPen( Qt::darkMagenta ), QSize( 5, 5 ) ) );
    d_curves[i].setStyle( QwtPlotCurve::NoCurve );
    i++;


    //
    // attach data
    //
    for( i = 0; i < CurvCnt; i++ )
        d_curves[i].setRawSamples( xval, yval, Size );
}

void MainWin::shiftDown( QRect &rect, int offset ) const
{
    rect.translate( 0, offset );
}

void MainWin::paintEvent( QPaintEvent *event )
{
    QFrame::paintEvent( event );

    QPainter painter( this );
    painter.setClipRect( contentsRect() );
    drawContents( &painter );
}


//
//  REDRAW CONTENTS
//
void MainWin::drawContents( QPainter *painter )
{
    int deltay, i;

    QRect r = contentsRect();

    deltay = r.height() / CurvCnt - 1;

    r.setHeight( deltay );

    //
    //  draw curves
    //
    for ( i = 0; i < CurvCnt; i++ )
    {
        xMap.setPaintInterval( r.left(), r.right() );
        yMap.setPaintInterval( r.top(), r.bottom() );

        painter->setRenderHint( QPainter::Antialiasing,
            d_curves[i].testRenderHint( QwtPlotItem::RenderAntialiased ) );
        d_curves[i].draw( painter, xMap, yMap, r );

        shiftDown( r, deltay );
    }

    //
    // draw titles
    //
    r = contentsRect();     // reset r
    painter->setFont( QFont( "Helvetica", 8 ) );

    const int alignment = Qt::AlignTop | Qt::AlignHCenter;

    painter->setPen( Qt::black );

    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Line/Fitted, Symbol: Cross" );
    shiftDown( r, deltay );

    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Sticks, Symbol: Ellipse" );
    shiftDown( r, deltay );

    painter->drawText( 0 , r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Lines, Symbol: None" );
    shiftDown( r, deltay );

    painter->drawText( 0 , r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Lines, Symbol: None, Antialiased" );
    shiftDown( r, deltay );

    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Steps, Symbol: None" );
    shiftDown( r, deltay );

    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: NoCurve, Symbol: XCross" );
}

int main ( int argc, char **argv )
{
    QApplication a( argc, argv );

    MainWin w;

    w.resize( 300, 600 );
    w.show();

    return a.exec();
}
