#ifndef _RANDOMPLOT_H_
#define _RANDOMPLOT_H_ 1

#include "incrementalplot.h"
#include <qdatetime.h>

class QTimer;

class RandomPlot: public IncrementalPlot
{
    Q_OBJECT

public:
    RandomPlot( QWidget *parent );

    virtual QSize sizeHint() const;

Q_SIGNALS:
    void running( bool );
    void elapsed( int ms );

public Q_SLOTS:
    void clear();
    void stop();
    void append( int timeout, int count );

private Q_SLOTS:
    void appendPoint();

private:
    void initCurve();

    QTimer *d_timer;
    int d_timerCount;

    QTime d_timeStamp;
};

#endif // _RANDOMPLOT_H_
