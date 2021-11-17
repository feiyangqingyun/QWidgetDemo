#ifndef FRMDEVICEBUTTON_H
#define FRMDEVICEBUTTON_H

#include <QWidget>

class DeviceButton;
class QPushButton;

namespace Ui {
class frmDeviceButton;
}

class frmDeviceButton : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceButton(QWidget *parent = 0);
    ~frmDeviceButton();

private slots:
    void initForm();
    void changeStyle();
    void changeColor();
    void on_ckCanMove_stateChanged(int arg1);

private:
    Ui::frmDeviceButton *ui;
    DeviceButton *btn1;
    DeviceButton *btn2;
    DeviceButton *btn3;
    QList<QPushButton *> btnStyle;
    QList<QPushButton *> btnColor;
};

#endif // FRMDEVICEBUTTON_H
