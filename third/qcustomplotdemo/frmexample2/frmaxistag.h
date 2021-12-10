#ifndef FRMAXISTAG_H
#define FRMAXISTAG_H

#include <QWidget>
#include "qcustomplot.h"
#include "axistag.h"

namespace Ui {
class frmAxisTag;
}

class frmAxisTag : public QWidget
{
    Q_OBJECT

public:
    explicit frmAxisTag(QWidget *parent = 0);
    ~frmAxisTag();

private:
    Ui::frmAxisTag *ui;
    QCustomPlot *mPlot;
    QPointer<QCPGraph> mGraph1;
    QPointer<QCPGraph> mGraph2;
    AxisTag *mTag1;
    AxisTag *mTag2;
    QTimer mDataTimer;

private slots:
    void initForm();
    void timerSlot();
};

#endif // FRMAXISTAG_H
