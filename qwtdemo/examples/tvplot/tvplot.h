#ifndef _TV_PLOT_H_

#include <qwt_plot.h>

class TVPlot: public QwtPlot
{
    Q_OBJECT

public:
    TVPlot( QWidget * = NULL );

public Q_SLOTS:
    void setMode( int );
    void exportPlot();

private:
    void populate();

private Q_SLOTS:
    void showItem( const QVariant &, bool on );
};

#endif
