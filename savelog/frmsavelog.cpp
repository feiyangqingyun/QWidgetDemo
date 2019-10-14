#pragma execution_character_set("utf-8")

#include "frmsavelog.h"
#include "ui_frmsavelog.h"
#include "savelog.h"
#include "qdatetime.h"
#include "qtimer.h"
#include "qdebug.h"

frmSaveLog::frmSaveLog(QWidget *parent) : QWidget(parent), ui(new Ui::frmSaveLog)
{
    ui->setupUi(this);
    this->initForm();
}

frmSaveLog::~frmSaveLog()
{
    delete ui;
}

void frmSaveLog::initForm()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(append()));
    timer->setInterval(1000);

    SaveLog::Instance()->setPath(qApp->applicationDirPath());
}

void frmSaveLog::append()
{
    QString msg = QString("当前时间: %1  测试打印输出消息").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    ui->txtMain->append(msg);
    qDebug() << msg;
}

void frmSaveLog::on_btnDebug_clicked()
{
    QString msg = QString("当前时间: %1  手动插入消息").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    ui->txtMain->append(msg);
    qDebug() << msg;
}

void frmSaveLog::on_ckTimer_stateChanged(int arg1)
{
    if (arg1 == 0) {
        timer->stop();
    } else {
        timer->start();
    }
}

void frmSaveLog::on_ckNet_stateChanged(int arg1)
{
    SaveLog::Instance()->setToNet(arg1 != 0);
}

void frmSaveLog::on_ckSave_stateChanged(int arg1)
{
    if (arg1 == 0) {
        SaveLog::Instance()->stop();
    } else {
        SaveLog::Instance()->start();
    }
}
