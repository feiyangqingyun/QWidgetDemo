#include "frmtrayicon.h"
#include "ui_frmtrayicon.h"
#include "trayicon.h"

frmTrayIcon::frmTrayIcon(QWidget *parent) : QWidget(parent), ui(new Ui::frmTrayIcon)
{
    ui->setupUi(this);
    TrayIcon::Instance()->setIcon(":/main.ico");
    TrayIcon::Instance()->setMainWidget(this);
}

frmTrayIcon::~frmTrayIcon()
{
    TrayIcon::Instance()->setVisible(false);
    delete ui;
}

void frmTrayIcon::on_btnShow_clicked()
{
    TrayIcon::Instance()->setVisible(true);
    TrayIcon::Instance()->showMessage("自定义控件大全", "已经最小化到托盘,双击打开!");
}

void frmTrayIcon::on_btnHide_clicked()
{
    TrayIcon::Instance()->setVisible(false);
}
