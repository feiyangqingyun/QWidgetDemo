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

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::frmAxisTag *ui;
    QPointer<QCPGraph> mGraph1;
    QPointer<QCPGraph> mGraph2;
    AxisTag *mTag1;
    AxisTag *mTag2;
    QTimer dataTimer;

private slots:
    void initForm();
    void timerSlot();
};

#endif // FRMAXISTAG_H
