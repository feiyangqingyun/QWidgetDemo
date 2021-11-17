#include "frmzhtopy.h"
#include "ui_frmzhtopy.h"
#include "zhtopy.h"

frmZhToPY::frmZhToPY(QWidget *parent) : QWidget(parent), ui(new Ui::frmZhToPY)
{
    ui->setupUi(this);
    ZhToPY::Instance()->loadPY(":/data/zhtopy.txt");
}

frmZhToPY::~frmZhToPY()
{
    delete ui;
}

void frmZhToPY::on_btnPY_clicked()
{
    ui->txtResult->setText(ZhToPY::Instance()->zhToPY(ui->txtChinese->text()));
}

void frmZhToPY::on_btnJP_clicked()
{
    ui->txtResult->setText(ZhToPY::Instance()->zhToJP(ui->txtChinese->text()));
}
