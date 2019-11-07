#include <qframe.h>
#include <qpalette.h>

class QwtDial;
class QwtAnalogClock;
class SpeedoMeter;
class AttitudeIndicator;

class CockpitGrid: public QFrame
{
    Q_OBJECT

public:
    CockpitGrid( QWidget *parent = NULL );

private Q_SLOTS:
    void changeSpeed();
    void changeGradient();
    void changeAngle();

private:
    QPalette colorTheme( const QColor & ) const;
    QwtDial *createDial( int pos );

    QwtAnalogClock *d_clock;
    SpeedoMeter *d_speedo;
    AttitudeIndicator *d_ai;
};
