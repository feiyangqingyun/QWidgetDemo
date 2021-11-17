#pragma execution_character_set("utf-8")

#include "frmsaveruntime.h"
#include "ui_frmsaveruntime.h"
#include "qfile.h"
#include "saveruntime.h"
#include "qdebug.h"

frmSaveRunTime::frmSaveRunTime(QWidget *parent) : QWidget(parent), ui(new Ui::frmSaveRunTime)
{
    ui->setupUi(this);
    //设置文件存储目录
    SaveRunTime::Instance()->setPath(qApp->applicationDirPath() + "/log");
}

frmSaveRunTime::~frmSaveRunTime()
{
    delete ui;
}

void frmSaveRunTime::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == 0) {
        SaveRunTime::Instance()->stop();
    } else {
        SaveRunTime::Instance()->start();
    }
    on_btnOpen_clicked();
}

void frmSaveRunTime::on_btnAppend_clicked()
{
    SaveRunTime::Instance()->initLog();
    SaveRunTime::Instance()->appendLog();
    on_btnOpen_clicked();
}

void frmSaveRunTime::on_btnUpdate_clicked()
{
    SaveRunTime::Instance()->saveLog();
    on_btnOpen_clicked();
}

void frmSaveRunTime::on_btnOpen_clicked()
{
    QString path = qApp->applicationDirPath();
    QString name = qApp->applicationFilePath();
    QStringList list = name.split("/");
    name = list.at(list.count() - 1).split(".").at(0);

    QString fileName = QString("%1/log/%2_runtime_%3.txt").arg(path).arg(name).arg(QDate::currentDate().year());
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        ui->txtMain->setText(file.readAll());
    }
}
