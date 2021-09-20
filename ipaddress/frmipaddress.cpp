#pragma execution_character_set("utf-8")

#include "frmipaddress.h"
#include "ui_frmipaddress.h"
#include "qdebug.h"

frmIPAddress::frmIPAddress(QWidget *parent) : QWidget(parent), ui(new Ui::frmIPAddress)
{
    ui->setupUi(this);
    on_btnSetIP_clicked();
}

frmIPAddress::~frmIPAddress()
{
    delete ui;
}

void frmIPAddress::on_btnSetIP_clicked()
{
    ui->widgetIP->setIP("192.168.1.56");
}

void frmIPAddress::on_btnGetIP_clicked()
{
    qDebug() << ui->widgetIP->getIP();
}

void frmIPAddress::on_btnClear_clicked()
{
    ui->widgetIP->clear();
}
