#include <qlayout.h>
#include "tunerfrm.h"
#include "ampfrm.h"
#include "mainwindow.h"

MainWindow::MainWindow():
    QWidget()
{
    TunerFrame *frmTuner = new TunerFrame( this );
    frmTuner->setFrameStyle( QFrame::Panel | QFrame::Raised );

    AmpFrame *frmAmp = new AmpFrame( this );
    frmAmp->setFrameStyle( QFrame::Panel | QFrame::Raised );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    layout->addWidget( frmTuner );
    layout->addWidget( frmAmp );

    connect( frmTuner, SIGNAL( fieldChanged( double ) ),
        frmAmp, SLOT( setMaster( double ) ) );

    frmTuner->setFreq( 90.0 );

    setPalette( QPalette( QColor( 192, 192, 192 ) ) );
    updateGradient();
}

void MainWindow::resizeEvent( QResizeEvent * )
{
    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11
    updateGradient();
}

void MainWindow::updateGradient()
{
    QPalette pal = palette();

    const QColor buttonColor = pal.color( QPalette::Button );
    const QColor midLightColor = pal.color( QPalette::Midlight );

    QLinearGradient gradient( rect().topLeft(), rect().topRight() );
    gradient.setColorAt( 0.0, midLightColor );
    gradient.setColorAt( 0.7, buttonColor );
    gradient.setColorAt( 1.0, buttonColor );

    pal.setBrush( QPalette::Window, gradient );
    setPalette( pal );
}
