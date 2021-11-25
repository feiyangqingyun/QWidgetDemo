#include "frmscreenwidget.h"
#include "ui_frmscreenwidget.h"
#include "screenwidget.h"

frmScreenWidget::frmScreenWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmScreenWidget)
{
    ui->setupUi(this);
}

frmScreenWidget::~frmScreenWidget()
{
    delete ui;
    exit(0); 
}

void frmScreenWidget::on_pushButton_clicked()
{
    ScreenWidget::Instance()->showFullScreen();
}
