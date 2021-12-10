#ifndef FRMSINCSCATTER_H
#define FRMSINCSCATTER_H

#include <QWidget>

namespace Ui {
class frmSincScatter;
}

class frmSincScatter : public QWidget
{
    Q_OBJECT

public:
    explicit frmSincScatter(QWidget *parent = 0);
    ~frmSincScatter();

private:
    Ui::frmSincScatter *ui;

private slots:
    void initForm();
};

#endif // FRMSINCSCATTER_H
