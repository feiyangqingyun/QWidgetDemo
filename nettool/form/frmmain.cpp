#include "frmmain.h"
#include "ui_frmmain.h"
#include "quiwidget.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(App::CurrentIndex);
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::on_tabWidget_currentChanged(int index)
{
    App::CurrentIndex = index;
    App::writeConfig();
}
