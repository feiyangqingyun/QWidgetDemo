#include <qframe.h>
class QwtCompass;

class CompassGrid: public QFrame
{
public:
    CompassGrid( QWidget *parent = NULL );

private:
    QwtCompass *createCompass( int pos );
};
