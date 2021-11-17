#pragma execution_character_set("utf-8")

#include "frmdevicesizetable.h"
#include "ui_frmdevicesizetable.h"

frmDeviceSizeTable::frmDeviceSizeTable(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceSizeTable)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(25);
}

frmDeviceSizeTable::~frmDeviceSizeTable()
{
    delete ui;
}
