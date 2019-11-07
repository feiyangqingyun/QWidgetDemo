#include <qframe.h>

class QwtWheel;
class QwtSlider;
class TuningThermo;

class TunerFrame : public QFrame
{
    Q_OBJECT
public:
    TunerFrame( QWidget *p );

Q_SIGNALS:
    void fieldChanged( double f );

public Q_SLOTS:
    void setFreq( double frq );

private Q_SLOTS:
    void adjustFreq( double frq );

private:
    QwtWheel *d_wheelFrequency;
    TuningThermo *d_thermoTune;
    QwtSlider *d_sliderFrequency;
};




