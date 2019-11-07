#include <qwidget.h>

class MainWindow : public QWidget
{
public:
    MainWindow();

protected:
    virtual void resizeEvent( QResizeEvent * );

private:
    void updateGradient();
};


