#include <qmainwindow.h>

class Plot;
class Panel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0 );

private Q_SLOTS:
    void updatePlot();
    void exportPlot();

private:
    Plot *d_plot;
    Panel *d_panel;
};
