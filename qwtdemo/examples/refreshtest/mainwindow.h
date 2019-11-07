#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <qmainwindow.h>

class Plot;
class Panel;
class QLabel;
class Settings;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = NULL );
    virtual bool eventFilter( QObject *, QEvent * );

private Q_SLOTS:
    void applySettings( const Settings & );

private:
    Plot *d_plot;
    Panel *d_panel;
    QLabel *d_frameCount;
};

#endif
