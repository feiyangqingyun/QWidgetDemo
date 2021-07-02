#include "frmdevicebutton.h"
#include "ui_frmdevicebutton.h"
#include "devicebutton.h"
#include "qdebug.h"

frmDeviceButton::frmDeviceButton(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceButton)
{
    ui->setupUi(this);
    this->initForm();
}

frmDeviceButton::~frmDeviceButton()
{
    delete ui;
}

void frmDeviceButton::initForm()
{
    //设置背景地图
    ui->labMap->setStyleSheet("border-image:url(:/image/bg_call.jpg);");

    btn1 = new DeviceButton(ui->labMap);
    btn1->setText("#1");
    btn1->setGeometry(5, 5, 35, 35);

    btn2 = new DeviceButton(ui->labMap);
    btn2->setText("#2");
    btn2->setGeometry(45, 5, 35, 35);

    btn3 = new DeviceButton(ui->labMap);
    btn3->setText("#3");
    btn3->setGeometry(85, 5, 35, 35);

    btnStyle << ui->btnCircle << ui->btnPolice << ui->btnBubble << ui->btnBubble2 << ui->btnMsg << ui->btnMsg2;
    foreach (QPushButton *btn, btnStyle) {
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(changeStyle()));
    }

    btnColor << ui->btnGreen << ui->btnBlue << ui->btnRed << ui->btnGray << ui->btnBlack << ui->btnPurple << ui->btnYellow;
    foreach (QPushButton *btn, btnColor) {
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(changeColor()));
    }
}

void frmDeviceButton::changeStyle()
{
    QPushButton *btn = (QPushButton *)sender();
    int index = btnStyle.indexOf(btn);
    DeviceButton::ButtonStyle style = (DeviceButton::ButtonStyle)index;
    btn1->setButtonStyle(style);
    btn2->setButtonStyle(style);
    btn3->setButtonStyle(style);
}

void frmDeviceButton::changeColor()
{
    QPushButton *btn = (QPushButton *)sender();
    int index = btnColor.indexOf(btn);
    DeviceButton::ButtonColor style = (DeviceButton::ButtonColor)index;
    btn1->setButtonColor(style);
    btn2->setButtonColor(style);
    btn3->setButtonColor(style);
}

void frmDeviceButton::on_ckCanMove_stateChanged(int arg1)
{
    bool canMove = (arg1 != 0);
    btn1->setCanMove(canMove);
    btn2->setCanMove(canMove);
    btn3->setCanMove(canMove);
}
