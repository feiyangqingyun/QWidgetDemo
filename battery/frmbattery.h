#ifndef FRMBATTERY_H
#define FRMBATTERY_H

#include <QWidget>

namespace Ui {
class frmBattery;
}

class frmBattery : public QWidget
{
    Q_OBJECT

public:
    explicit frmBattery(QWidget *parent = 0);
    ~frmBattery();

private:
    Ui::frmBattery *ui;

private slots:
    void initForm();
};

#endif // FRMBATTERY_H
