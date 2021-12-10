#ifndef FRMQUADRATIC_H
#define FRMQUADRATIC_H

#include <QWidget>

namespace Ui {
class frmQuadratic;
}

class frmQuadratic : public QWidget
{
    Q_OBJECT

public:
    explicit frmQuadratic(QWidget *parent = 0);
    ~frmQuadratic();

private:
    Ui::frmQuadratic *ui;

private slots:
    void initForm();
};

#endif // FRMQUADRATIC_H
