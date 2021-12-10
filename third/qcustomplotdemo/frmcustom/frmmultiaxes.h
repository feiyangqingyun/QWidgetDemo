#ifndef FRMMULTIAXES_H
#define FRMMULTIAXES_H

#include <QWidget>
#include "qcustomplot.h"

namespace Ui {
class frmMultiAxes;
}

class frmMultiAxes : public QWidget
{
    Q_OBJECT

public:
    explicit frmMultiAxes(QWidget *parent = 0);
    ~frmMultiAxes();

private:
    Ui::frmMultiAxes *ui;
    int count;

private slots:
    //初始化界面数据
    void initForm();
    //用户按下区域改变
    void selectionChangedByUser();
    //初始化图表控件
    void initPlot(QCustomPlot *customPlot, const QCPAxis::AxisType &type, int count);
    //加载图表控件
    void loadPlot(QCustomPlot *customPlot, const QCPAxis::AxisType &type, int count);
    //加载数据到图表控件
    void loadData(QCustomPlot *customPlot, int index);
};

#endif // FRMMULTIAXES_H
