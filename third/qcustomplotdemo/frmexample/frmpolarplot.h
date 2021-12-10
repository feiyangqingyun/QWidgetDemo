#ifndef FRMPOLARPLOT_H
#define FRMPOLARPLOT_H

#include <QWidget>

namespace Ui {
class frmPolarPlot;
}

class frmPolarPlot : public QWidget
{
    Q_OBJECT

public:
    explicit frmPolarPlot(QWidget *parent = 0);
    ~frmPolarPlot();

private:
    Ui::frmPolarPlot *ui;

private slots:
    void initForm();
};

#endif // FRMPOLARPLOT_H
