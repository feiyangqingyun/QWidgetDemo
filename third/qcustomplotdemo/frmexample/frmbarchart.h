#ifndef FRMBARCHART_H
#define FRMBARCHART_H

#include <QWidget>

namespace Ui {
class frmBarChart;
}

class frmBarChart : public QWidget
{
    Q_OBJECT

public:
    explicit frmBarChart(QWidget *parent = 0);
    ~frmBarChart();

private:
    Ui::frmBarChart *ui;

private slots:
    void initForm();
};

#endif // FRMBARCHART_H
