#pragma execution_character_set("utf-8")

#include "frmdevicesizetable.h"
#include "ui_frmdevicesizetable.h"

frmDeviceSizeTable::frmDeviceSizeTable(QWidget *parent) : QWidget(parent), ui(new Ui::frmDeviceSizeTable)
{
	ui->setupUi(this);
}

frmDeviceSizeTable::~frmDeviceSizeTable()
{
	delete ui;
}
