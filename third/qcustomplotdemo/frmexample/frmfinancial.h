#ifndef FRMFINANCIAL_H
#define FRMFINANCIAL_H

#include <QWidget>

namespace Ui {
class frmFinancial;
}

class frmFinancial : public QWidget
{
    Q_OBJECT

public:
    explicit frmFinancial(QWidget *parent = 0);
    ~frmFinancial();

private:
    Ui::frmFinancial *ui;

private slots:
    void initForm();
};

#endif // FRMFINANCIAL_H
