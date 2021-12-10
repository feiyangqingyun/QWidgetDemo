#ifndef FRMPARAMETRICCURVE_H
#define FRMPARAMETRICCURVE_H

#include <QWidget>

namespace Ui {
class frmParametricCurve;
}

class frmParametricCurve : public QWidget
{
    Q_OBJECT

public:
    explicit frmParametricCurve(QWidget *parent = 0);
    ~frmParametricCurve();

private:
    Ui::frmParametricCurve *ui;

private slots:
    void initForm();
};

#endif // FRMPARAMETRICCURVE_H
