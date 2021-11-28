#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "qtimer.h"
#include "qudpsocket.h"
#include "qsharedmemory.h"
#include "qprocess.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qdesktopservices.h"
#include "qmessagebox.h"
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include "qstandardpaths.h"
#endif

#include "app.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::changeEvent(QEvent *event)
{
    //隐藏当前界面,最小化到托盘
    if(event->type() == QEvent::WindowStateChange) {
        if(windowState() & Qt::WindowMinimized) {
            hide();
        }
    }

    QWidget::changeEvent(event);
}

void frmMain::initForm()
{
    count = 0;
    ok = false;

    //每秒钟定时询问心跳
    timerHeart = new QTimer(this);
    timerHeart->setInterval(2000);
    connect(timerHeart, SIGNAL(timeout()), this, SLOT(sendHearData()));

    //从6050端口开始,如果绑定失败则将端口加1,直到绑定成功
    udp = new QUdpSocket(this);
    int port = 6050;
    while(!udp->bind(port)) {
        port++;
    }

    connect(udp, SIGNAL(readyRead()), this, SLOT(readData()));

    if (App::TargetAppName.isEmpty()) {
        ui->btnStart->setText("启动");
        ui->btnStart->setEnabled(false);
        timerHeart->stop();
    } else {
        ui->btnStart->setText("暂停");
        ui->btnStart->setEnabled(true);
        timerHeart->start();
    }

    ui->txtAppName->setText(App::TargetAppName);
    ui->txtAppName->setFocus();
}

void frmMain::sendHearData()
{
    udp->writeDatagram("hello", QHostAddress::LocalHost, App::TargetAppPort);

    //判断当前是否没有回复
    if (!ok) {
        count++;
    } else {
        count = 0;
        ok = false;
    }

    //如果超过规定次数没有收到心跳回复,则超时重启
    if (count >= App::TimeoutCount) {
        timerHeart->stop();

        QSharedMemory mem(App::TargetAppName);
        if (!mem.create(1)) {
            killApp();
        }

        QTimer::singleShot(1000 , this, SLOT(killOther()));
        QTimer::singleShot(3000 , this, SLOT(startApp()));
        QTimer::singleShot(4000 , this, SLOT(startExplorer()));
    }
}

void frmMain::killApp()
{
    QProcess *p = new QProcess;
    p->start(QString("taskkill /im %1.exe /f").arg(App::TargetAppName));
}

void frmMain::killOther()
{
    QProcess *p = new QProcess;
    p->start(QString("taskkill /im %1.exe /f").arg("WerFault"));

    //重建缓存,彻底清除托盘图标
    if (App::ReStartExplorer) {
        QProcess *p1 = new QProcess;
        p1->start("taskkill /f /im explorer.exe");
    }
}

void frmMain::startApp()
{
    if (ui->btnStart->text() == "开始" || ui->btnStart->text() == "启动") {
        count = 0;
        return;
    }

    QProcess *p = new QProcess;
    p->start(QString("\"%1/%2.exe\"").arg(qApp->applicationDirPath()).arg(App::TargetAppName));

    count = 0;
    ok = true;
    timerHeart->start();

    App::ReStartCount++;
    App::ReStartLastTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    App::writeConfig();

    ui->labCount->setText(QString("已重启 %1 次").arg(App::ReStartCount));
    ui->labInfo->setText(QString("最后一次重启在 %1").arg(App::ReStartLastTime));
}

void frmMain::startExplorer()
{
    //取得操作系统目录路径,指定操作系统目录下的explorer程序,采用绝对路径,否则在64位操作系统下无效
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    QString str = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#else
    QString str = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
#endif

    if (App::ReStartExplorer) {
        str = QString("%1\\Windows\\explorer.exe").arg(str.mid(0, 2));
        QProcess *p = new QProcess(this);
        p->start(str);
    }
}

void frmMain::readData()
{
    QByteArray tempData;
    do {
        tempData.resize(udp->pendingDatagramSize());
        udp->readDatagram(tempData.data(), tempData.size());
        QString data = QLatin1String(tempData);
        if (data.right(2) == "OK") {
            count = 0;
            ok = true;
        }
    } while (udp->hasPendingDatagrams());
}

void frmMain::on_btnOk_clicked()
{
    App::TargetAppName = ui->txtAppName->text();
    if (App::TargetAppName == "") {
        QMessageBox::critical(this, "提示", "应用程序名称不能为空!");
        ui->txtAppName->setFocus();
        return;
    }

    App::writeConfig();
    ui->btnStart->setEnabled(true);
}

void frmMain::on_btnStart_clicked()
{
    count = 0;
    if (ui->btnStart->text() == "暂停") {
        timerHeart->stop();
        ui->btnStart->setText("开始");
    } else {
        timerHeart->start();
        ui->btnStart->setText("暂停");
    }
}

void frmMain::on_btnReset_clicked()
{
    App::ReStartCount = 0;
    App::ReStartLastTime = "2019-01-01 12:00:00";
    App::writeConfig();

    ui->txtAppName->setText(App::TargetAppName);
    ui->labCount->setText(QString("已重启 %1 次").arg(App::ReStartCount));
    ui->labInfo->setText(QString("最后一次重启在 %1").arg(App::ReStartLastTime));
    QMessageBox::information(this, "提示", "重置配置文件成功!");
}
