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
    //启动定时器追加数据
    count = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(append()));
    timer->setInterval(100);

    //添加消息类型
    QStringList types, datas;
    types << "Debug" << "Info" << "Warning" << "Critical" << "Fatal";
    datas << "1" << "2" << "4" << "8" << "16";
    ui->cboxType->addItems(types);

    //添加消息类型到列表用于勾选设置哪些类型需要重定向
    int count = types.count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(types.at(i));
        item->setData(Qt::UserRole, datas.at(i));
        item->setCheckState(Qt::Checked);
        ui->listType->addItem(item);
    }

    //添加日志文件大小下拉框
    ui->cboxSize->addItem("不启用", 0);
    ui->cboxSize->addItem("5kb", 5);
    ui->cboxSize->addItem("10kb", 10);
    ui->cboxSize->addItem("30kb", 30);
    ui->cboxSize->addItem("1mb", 1024);

    ui->cboxRow->addItem("不启用", 0);
    ui->cboxRow->addItem("100条", 100);
    ui->cboxRow->addItem("500条", 500);
    ui->cboxRow->addItem("2000条", 2000);
    ui->cboxRow->addItem("10000条", 10000);

    //设置是否开启日志上下文打印比如行号、函数等
    SaveLog::Instance()->setUseContext(false);
    //设置文件存储目录
    SaveLog::Instance()->setPath(qApp->applicationDirPath() + "/log");
}

void frmSaveLog::append(const QString &flag)
{
    if (count >= 100) {
        count = 0;
        ui->txtMain->clear();
    }

    QString str1;
    int type = ui->cboxType->currentIndex();
    if (!ui->ckSave->isChecked()) {
        if (type == 0) {
            str1 = "Debug ";
        } else if (type == 1) {
            str1 = "Infox ";
        } else if (type == 2) {
            str1 = "Warnx ";
        } else if (type == 3) {
            str1 = "Error ";
        } else if (type == 4) {
            str1 = "Fatal ";
        }
    }

    QString str2 = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString str3 = flag.isEmpty() ? "自动插入消息" : flag;
    QString msg = QString("%1当前时间: %2 %3").arg(str1).arg(str2).arg(str3);

    //开启网络重定向换成英文方便接收解析不乱码
    //对方接收解析的工具未必是utf8
    if (ui->ckNet->isChecked()) {
        msg = QString("%1time: %2 %3").arg(str1).arg(str2).arg("(QQ: 517216493 WX: feiyangqingyun)");
    }

    count++;
    ui->txtMain->append(msg);

    //根据不同的类型打印
    //TMD转换要分两部走不然msvc的debug版本会乱码(英文也一样)
    //char *data = msg.toUtf8().data();
    QByteArray buffer = msg.toUtf8();
    const char *data = buffer.constData();
    if (type == 0) {
        qDebug(data);
    } else if (type == 1) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        qInfo(data);
#endif
    } else if (type == 2) {
        qWarning(data);
    } else if (type == 3) {
        qCritical(data);
    } else if (type == 4) {
        //调用下面这个打印完会直接退出程序
        qFatal(data);
    }
}

void frmSaveLog::on_btnLog_clicked()
{
    append("手动插入消息");
}

void frmSaveLog::on_ckTimer_stateChanged(int arg1)
{
    if (arg1 == 0) {
        timer->stop();
    } else {
        timer->start();
        on_btnLog_clicked();
    }
}

void frmSaveLog::on_ckNet_stateChanged(int arg1)
{
    SaveLog::Instance()->setListenPort(ui->txtPort->text().toInt());
    SaveLog::Instance()->setToNet(ui->ckNet->isChecked());
    on_btnLog_clicked();
}

void frmSaveLog::on_ckSave_stateChanged(int arg1)
{
    if (arg1 == 0) {
        SaveLog::Instance()->stop();
    } else {
        SaveLog::Instance()->start();
        on_btnLog_clicked();
    }
}

void frmSaveLog::on_cboxSize_currentIndexChanged(int index)
{
    int size = ui->cboxSize->itemData(index).toInt();
    SaveLog::Instance()->setMaxSize(size);
    on_btnLog_clicked();
}

void frmSaveLog::on_cboxRow_currentIndexChanged(int index)
{
    int row = ui->cboxRow->itemData(index).toInt();
    SaveLog::Instance()->setMaxRow(row);
    on_btnLog_clicked();
}

void frmSaveLog::on_listType_itemPressed(QListWidgetItem *item)
{
    //切换选中行状态
    item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);

    //找到所有勾选的类型进行设置
    quint8 types = 0;
    int count = ui->listType->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = ui->listType->item(i);
        if (item->checkState() == Qt::Checked) {
            types += item->data(Qt::UserRole).toInt();
        }
    }

    SaveLog::Instance()->setMsgType((MsgType)types);
}
