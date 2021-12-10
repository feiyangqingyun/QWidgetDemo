#ifndef FRMDATE_H
#define FRMDATE_H

#include <QWidget>

namespace Ui {
class frmDate;
}

class frmDate : public QWidget
{
    Q_OBJECT

public:
    explicit frmDate(QWidget *parent = 0);
    ~frmDate();

private:
    Ui::frmDate *ui;

private slots:
    void initForm();
};

#endif // FRMDATE_H
