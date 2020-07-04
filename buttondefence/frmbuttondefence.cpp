#include "frmbuttondefence.h"
#include "ui_frmbuttondefence.h"
#include "buttondefence.h"
#include "qdebug.h"

frmButtonDefence::frmButtonDefence(QWidget *parent) : QWidget(parent), ui(new Ui::frmButtonDefence)
{
    ui->setupUi(this);
    this->initForm();
}

frmButtonDefence::~frmButtonDefence()
{
    delete ui;
}

void frmButtonDefence::initForm()
{
    //设置背景地图
    ui->labMap->setStyleSheet("border-image:url(:/image/bg_call.jpg);");

    btn1 = new ButtonDefence(ui->labMap);
    btn1->setText("#1");
    btn1->setGeometry(5, 5, 35, 35);

    btn2 = new ButtonDefence(ui->labMap);
    btn2->setText("#2");
    btn2->setGeometry(45, 5, 35, 35);

    btn3 = new ButtonDefence(ui->labMap);
    btn3->setText("#3");
    btn3->setGeometry(85, 5, 35, 35);

    btnStyle << ui->btnCircle << ui->btnPolice << ui->btnBubble << ui->btnBubble2 << ui->btnMsg << ui->btnMsg2 << ui->btnCustom;
    foreach (QPushButton *btn, btnStyle) {
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(changeStyle()));
    }

    btnStatus << ui->btnArming << ui->btnDisarming << ui->btnAlarm << ui->btnBypass << ui->btnError;
    foreach (QPushButton *btn, btnStatus) {
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(changeStatus()));
    }
}

void frmButtonDefence::changeStyle()
{
    QPushButton *btn = (QPushButton *)sender();
    int index = btnStyle.indexOf(btn);
    ButtonDefence::ButtonStyle style = (ButtonDefence::ButtonStyle)index;
    btn1->setButtonStyle(style);
    btn2->setButtonStyle(style);
    btn3->setButtonStyle(style);

    if (index == 6) {
        btn1->setText("");
        btn2->setText("");
        btn3->setText("");
    } else {
        btn1->setText("#1");
        btn2->setText("#2");
        btn3->setText("#3");
    }
}

void frmButtonDefence::changeStatus()
{
    QPushButton *btn = (QPushButton *)sender();
    int index = btnStatus.indexOf(btn);
    ButtonDefence::ButtonStatus style = (ButtonDefence::ButtonStatus)index;
    btn1->setButtonStatus(style);
    btn2->setButtonStatus(style);
    btn3->setButtonStatus(style);
}

void frmButtonDefence::on_ckCanMove_stateChanged(int arg1)
{
    bool canMove = (arg1 != 0);
    btn1->setCanMove(canMove);
    btn2->setCanMove(canMove);
    btn3->setCanMove(canMove);
}

void frmButtonDefence::on_btnPoint_clicked()
{
    qDebug() << "btn1" << "x" << btn1->geometry().x() << "y" << btn1->geometry().y();
    qDebug() << "btn2" << "x" << btn2->geometry().x() << "y" << btn2->geometry().y();
    qDebug() << "btn3" << "x" << btn3->geometry().x() << "y" << btn3->geometry().y();
}
