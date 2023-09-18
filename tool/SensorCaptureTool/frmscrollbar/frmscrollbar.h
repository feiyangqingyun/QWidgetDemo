#ifndef FRMSCROLLBAR_H
#define FRMSCROLLBAR_H

#include <QWidget>
#include "qcustomplot.h"

namespace Ui {
class frmScrollBar;
}

class frmScrollBar : public QWidget
{
    Q_OBJECT

public:
    explicit frmScrollBar(QWidget *parent = 0);
    ~frmScrollBar();

    void updateGraph0Data(QVector<double> x, QVector<double> y);
    void updateGraph1Data(QVector<double> x, QVector<double> y);
    void updateGraph(void);

private:
    Ui::frmScrollBar *ui;

private slots:
    void initForm();
    void horzScrollBarChanged(int value);
    void vertScrollBarChanged(int value);
    void xAxisChanged(QCPRange range);
    void yAxisChanged(QCPRange range);
};

#endif // FRMSCROLLBAR_H
