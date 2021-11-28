#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "appkey.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::on_btnAdd_clicked()
{
    QString name = ui->lineEdit->text().trimmed();
    ui->listWidget->addItem(name);

    //计算当前设备数量多少
    AppKey::Instance()->checkCount(ui->listWidget->count());
}
