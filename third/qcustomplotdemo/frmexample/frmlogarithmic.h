#ifndef FRMLOGARITHMIC_H
#define FRMLOGARITHMIC_H

#include <QWidget>

namespace Ui {
class frmLogarithmic;
}

class frmLogarithmic : public QWidget
{
    Q_OBJECT

public:
    explicit frmLogarithmic(QWidget *parent = 0);
    ~frmLogarithmic();

private:
    Ui::frmLogarithmic *ui;

private slots:
    void initForm();
};

#endif // FRMLOGARITHMIC_H
