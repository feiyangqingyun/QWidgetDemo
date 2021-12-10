#ifndef FRMMULTIAXIS_H
#define FRMMULTIAXIS_H

#include <QWidget>

namespace Ui {
class frmMultiAxis;
}

class frmMultiAxis : public QWidget
{
    Q_OBJECT

public:
    explicit frmMultiAxis(QWidget *parent = 0);
    ~frmMultiAxis();

private:
    Ui::frmMultiAxis *ui;

private slots:
    void initForm();
};

#endif // FRMMULTIAXIS_H
