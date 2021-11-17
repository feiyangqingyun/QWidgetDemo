#ifndef _SLIDER_TAB_H
#define _SLIDER_TAB_H 1

#include <qwidget.h>

class QBoxLayout;

class SliderTab: public QWidget
{
public:
    SliderTab( QWidget *parent = NULL );

private:
    QBoxLayout *createLayout( Qt::Orientation,
        QWidget *widget = NULL );
};

#endif
