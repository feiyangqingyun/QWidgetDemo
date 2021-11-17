#ifndef _KNOB_BOX_H_
#define _KNOB_BOX_H_

#include <qwidget.h>

class QLabel;
class QwtKnob;

class KnobBox: public QWidget
{
    Q_OBJECT
public:
    KnobBox( QWidget *parent, int knobType );

private Q_SLOTS:
    void setNum( double v );

private:
    QwtKnob *createKnob( int knobType ) const;

    QwtKnob *d_knob;
    QLabel *d_label;
};

#endif
