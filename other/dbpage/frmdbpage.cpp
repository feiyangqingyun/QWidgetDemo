#pragma execution_character_set("utf-8")

#include "frmdbpage.h"
#include "ui_frmdbpage.h"
#include "dbpage.h"

frmDbPage::frmDbPage(QWidget *parent) : QWidget(parent), ui(new Ui::frmDbPage)
{
    ui->setupUi(this);
    this->initForm();
    on_btnSelect_clicked();
}

frmDbPage::~frmDbPage()
{
    delete ui;
}

void frmDbPage::initForm()
{
    columnNames.clear();
    columnWidths.clear();

    tableName = "LogInfo";
    countName = "rowid";

    columnNames.append("防区号");
    columnNames.append("防区名称");
    columnNames.append("设备IP");
    columnNames.append("日志类型");
    columnNames.append("事件内容");
    columnNames.append("触发时间");
    columnNames.append("告警详情");
    columnNames.append("告警数据");
    columnNames.append("告警图像");

    columnWidths.append(70);
    columnWidths.append(120);
    columnWidths.append(120);
    columnWidths.append(80);
    columnWidths.append(150);
    columnWidths.append(160);
    columnWidths.append(160);
    columnWidths.append(160);
    columnWidths.append(160);

    //设置需要显示数据的表格和翻页的按钮
    dbPage = new DbPage(this);
    //设置所有列居中显示
    dbPage->setAllCenter(true);
    dbPage->setControl(ui->tableMain, ui->labPageTotal, ui->labPageCurrent, ui->labRecordsTotal, ui->labRecordsPerpage,
                       ui->labSelectTime, 0, ui->btnFirst, ui->btnPreVious, ui->btnNext, ui->btnLast, countName);
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);
    ui->tableMain->verticalHeader()->setDefaultSectionSize(25);
}

void frmDbPage::on_btnSelect_clicked()
{
    //绑定数据到表格
    QString sql = "where 1=1";
    dbPage->setTableName(tableName);
    dbPage->setOrderSql(QString("%1 %2").arg(countName).arg("asc"));
    dbPage->setWhereSql(sql);
    dbPage->setRecordsPerpage(20);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->select();
}
