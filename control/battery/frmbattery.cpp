#pragma execution_character_set("utf-8")

#include "frmbattery.h"
#include "ui_frmbattery.h"

frmBattery::frmBattery(QWidget *parent) : QWidget(parent), ui(new Ui::frmBattery)
{
    ui->setupUi(this);
    this->initForm();
}

frmBattery::~frmBattery()
{
    delete ui;
}

void frmBattery::initForm()
{
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->battery, SLOT(setValue(int)));
    ui->horizontalSlider->setValue(30);
}
