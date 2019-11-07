#ifndef _WHEEL_BOX_H_
#define _WHEEL_BOX_H_ 1

#include <qwidget.h>

class QLabel;
class QwtThermo;
class QwtWheel;

class WheelBox: public QWidget
{
    Q_OBJECT
public:
    WheelBox( Qt::Orientation, 
        int type, QWidget *parent = NULL );

private Q_SLOTS:
    void setNum( double v );

private:
    QWidget *createBox( Qt::Orientation, int type );

private:
    QwtWheel *d_wheel;
    QwtThermo *d_thermo;
    QLabel *d_label;
};

#endif
