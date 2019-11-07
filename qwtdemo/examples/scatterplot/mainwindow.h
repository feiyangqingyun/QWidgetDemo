#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_ 1

#include <qmainwindow.h>

class Plot;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    void setSamples( int samples );

private:
    Plot *d_plot;
};

#endif
