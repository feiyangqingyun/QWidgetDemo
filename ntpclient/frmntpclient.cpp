#pragma execution_character_set("utf-8")

#include "frmntpclient.h"
#include "ui_frmntpclient.h"
#include "ntpclient.h"
#include "qdebug.h"

frmNtpClient::frmNtpClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmNtpClient)
{
    ui->setupUi(this);
    ui->txtNtpIP->setText("ntp1.aliyun.com");
    connect(NtpClient::Instance(), SIGNAL(receiveTime(QDateTime)), this, SLOT(receiveTime(QDateTime)));
}

frmNtpClient::~frmNtpClient()
{
    delete ui;
}

void frmNtpClient::on_btnGetTime_clicked()
{
    NtpClient::Instance()->setNtpIP(ui->txtNtpIP->text().trimmed());
    NtpClient::Instance()->getDateTime();
}

void frmNtpClient::receiveTime(const QDateTime &dateTime)
{
    ui->txtTime->setText(dateTime.toString("yyyy-MM-dd HH:mm:ss zzz"));
}
