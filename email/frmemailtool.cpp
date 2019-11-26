#pragma execution_character_set("utf-8")

#include "frmemailtool.h"
#include "ui_frmemailtool.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "sendemailthread.h"

frmEmailTool::frmEmailTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmEmailTool)
{
    ui->setupUi(this);
    this->initForm();
}

frmEmailTool::~frmEmailTool()
{
    delete ui;
}

void frmEmailTool::initForm()
{    
    ui->cboxServer->setCurrentIndex(1);
    connect(SendEmailThread::Instance(), SIGNAL(receiveEmailResult(QString)),
            this, SLOT(receiveEmailResult(QString)));
    SendEmailThread::Instance()->start();
}

void frmEmailTool::on_btnSend_clicked()
{
    if (!check()) {
        return;
    }

    SendEmailThread::Instance()->setEmailTitle(ui->txtTitle->text());
    SendEmailThread::Instance()->setSendEmailAddr(ui->txtSenderAddr->text());
    SendEmailThread::Instance()->setSendEmailPwd(ui->txtSenderPwd->text());
    SendEmailThread::Instance()->setReceiveEmailAddr(ui->txtReceiverAddr->text());

    //设置好上述配置后,以后只要调用Append方法即可发送邮件
    SendEmailThread::Instance()->append(ui->txtContent->toHtml(), ui->txtFileName->text());
}

void frmEmailTool::on_btnSelect_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec()) {
        ui->txtFileName->clear();
        QStringList files = dialog.selectedFiles();
        ui->txtFileName->setText(files.join(";"));
    }
}

bool frmEmailTool::check()
{
    if (ui->txtSenderAddr->text() == "") {
        QMessageBox::critical(this, "错误", "用户名不能为空!");
        ui->txtSenderAddr->setFocus();
        return false;
    }

    if (ui->txtSenderPwd->text() == "") {
        QMessageBox::critical(this, "错误", "用户密码不能为空!");
        ui->txtSenderPwd->setFocus();
        return false;
    }

    if (ui->txtSenderAddr->text() == "") {
        QMessageBox::critical(this, "错误", "发件人不能为空!");
        ui->txtSenderAddr->setFocus();
        return false;
    }

    if (ui->txtReceiverAddr->text() == "") {
        QMessageBox::critical(this, "错误", "收件人不能为空!");
        ui->txtReceiverAddr->setFocus();
        return false;
    }

    if (ui->txtTitle->text() == "") {
        QMessageBox::critical(this, "错误", "邮件标题不能为空!");
        ui->txtTitle->setFocus();
        return false;
    }

    return true;
}

void frmEmailTool::on_cboxServer_currentIndexChanged(int index)
{
    if (index == 2) {
        ui->cboxPort->setCurrentIndex(1);
        ui->ckSSL->setChecked(true);
    } else {
        ui->cboxPort->setCurrentIndex(0);
        ui->ckSSL->setChecked(false);
    }
}

void frmEmailTool::receiveEmailResult(QString result)
{
    QMessageBox::information(this, "提示", result);
}
