#ifndef _DIAL_BOX_H_
#define _DIAL_BOX_H_

#include <qwidget.h>

class QLabel;
class QwtDial;

class DialBox: public QWidget
{
    Q_OBJECT
public:
    DialBox( QWidget *parent, int type );

private Q_SLOTS:
    void setNum( double v );

private:
    QwtDial *createDial( int type ) const;

    QwtDial *d_dial;
    QLabel *d_label;
};

#endif
