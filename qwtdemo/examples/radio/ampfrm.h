#include <qframe.h>

class Knob;
class Thermo;

class AmpFrame : public QFrame
{
    Q_OBJECT
public:
    AmpFrame( QWidget * );

public Q_SLOTS:
    void setMaster( double v );

protected:
    void timerEvent( QTimerEvent * );

private:
    Knob *d_knbVolume;
    Knob *d_knbBalance;
    Knob *d_knbTreble;
    Knob *d_knbBass;
    Thermo *d_thmLeft;
    Thermo *d_thmRight;
    double d_master;
};



