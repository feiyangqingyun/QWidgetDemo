#pragma execution_character_set("utf-8")

#include "frmlightbutton.h"
#include "ui_frmlightbutton.h"
#include "qdatetime.h"
#include "qtimer.h"

frmLightButton::frmLightButton(QWidget *parent) : QWidget(parent), ui(new Ui::frmLightButton)
{
    ui->setupUi(this);
    this->initForm();
}

frmLightButton::~frmLightButton()
{
    delete ui;
}

void frmLightButton::initForm()
{
    ui->lightButton2->setBgColor(QColor(255, 107, 107));
    ui->lightButton3->setBgColor(QColor(24, 189, 155));

    type = 0;

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));
    timer->start();
    updateValue();

    //以下方法启动报警
    //ui->lightButton1->setAlarmColor(QColor(255, 0, 0));
    //ui->lightButton1->setNormalColor(QColor(0, 0, 0));
    //ui->lightButton1->startAlarm();
}

void frmLightButton::updateValue()
{
    if (type == 0) {
        ui->lightButton1->setLightGreen();
        ui->lightButton2->setLightRed();
        ui->lightButton3->setLightBlue();
        type = 1;
    } else if (type == 1) {
        ui->lightButton1->setLightBlue();
        ui->lightButton2->setLightGreen();
        ui->lightButton3->setLightRed();
        type = 2;
    } else if (type == 2) {
        ui->lightButton1->setLightRed();
        ui->lightButton2->setLightBlue();
        ui->lightButton3->setLightGreen();
        type = 0;
    }
}
