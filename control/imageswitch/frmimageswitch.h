#ifndef FRMIMAGESWITCH_H
#define FRMIMAGESWITCH_H

#include <QWidget>

namespace Ui {
class frmImageSwitch;
}

class frmImageSwitch : public QWidget
{
    Q_OBJECT

public:
    explicit frmImageSwitch(QWidget *parent = 0);
    ~frmImageSwitch();

private:
    Ui::frmImageSwitch *ui;

private slots:
    void initForm();
    void checkedChanged(bool checked);
};

#endif // FRMIMAGESWITCH_H
