#include "frmmain.h"
#include "ui_frmmain.h"
#include "qwt.h"
#include "qwt_dial.h"
#include "qwt_plot.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
}

frmMain::~frmMain()
{
    delete ui;
}
