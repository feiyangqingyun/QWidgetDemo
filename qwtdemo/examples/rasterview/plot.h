#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget * = NULL );

public Q_SLOTS:
    void exportPlot();
    void setResampleMode( int );

private:
    QwtPlotSpectrogram *d_spectrogram;
};
