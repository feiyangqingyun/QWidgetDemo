#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "qmessagebox.h"
#include "qfile.h"
#include "qprocess.h"
#include "qdebug.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    qDebug() << this->getCpuName() << this->getCpuId() << this->getDiskNum();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    QStringList min;
    min << "1" << "5" << "10" << "20" << "30";
    for (int i = 1; i <= 24; i++) {
        min << QString::number(i * 60);
    }

    ui->cboxMin->addItems(min);
    ui->cboxMin->setCurrentIndex(1);
    ui->dateEdit->setDate(QDate::currentDate());

    for (int i = 5; i <= 150; i = i + 5) {
        ui->cboxCount->addItem(QString("%1").arg(i));
    }
}

QString frmMain::getWMIC(const QString &cmd)
{
    //获取cpu名称：wmic cpu get Name
    //获取cpu核心数：wmic cpu get NumberOfCores
    //获取cpu线程数：wmic cpu get NumberOfLogicalProcessors
    //查询cpu序列号：wmic cpu get processorid
    //查询主板序列号：wmic baseboard get serialnumber
    //查询BIOS序列号：wmic bios get serialnumber
    //查看硬盘：wmic diskdrive get serialnumber
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    QStringList list = cmd.split(" ");
    result = result.remove(list.last(), Qt::CaseInsensitive);
    result = result.replace("\r", "");
    result = result.replace("\n", "");
    result = result.simplified();
    return result;
}

QString frmMain::getCpuName()
{
    return getWMIC("wmic cpu get name");
}

QString frmMain::getCpuId()
{
    return getWMIC("wmic cpu get processorid");
}

QString frmMain::getDiskNum()
{
    return getWMIC("wmic diskdrive where index=0 get serialnumber");
}

QString frmMain::getXorEncryptDecrypt(const QString &data, char key)
{
    //采用异或加密,也可以自行更改算法
    QByteArray buffer = data.toLatin1();
    int size = buffer.size();
    for (int i = 0; i < size; i++) {
        buffer[i] = buffer.at(i) ^ key;
    }

    return QLatin1String(buffer);
}

void frmMain::on_btnOk_clicked()
{
    bool useDate = ui->ckDate->isChecked();
    bool useRun = ui->ckRun->isChecked();
    bool useCount = ui->ckCount->isChecked();

    if (!useDate && !useRun && !useCount) {
        if (QMessageBox::question(this, "询问", "确定要生成没有任何限制的密钥吗?") != QMessageBox::Yes) {
            return;
        }
    }

    QString strDate = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString strRun = ui->cboxMin->currentText();
    QString strCount = ui->cboxCount->currentText();
    QString key = QString("%1|%2|%3|%4|%5|%6").arg(useDate).arg(strDate).arg(useRun).arg(strRun).arg(useCount).arg(strCount);

    QFile file(QApplication::applicationDirPath() + "/key.db");
    file.open(QFile::WriteOnly | QIODevice::Text);
    file.write(getXorEncryptDecrypt(key, 110).toLatin1());
    file.close();
    QMessageBox::information(this, "提示", "生成密钥成功,将 key.db 文件拷贝到对应目录即可!");
}

void frmMain::on_btnClose_clicked()
{
    this->close();
}
