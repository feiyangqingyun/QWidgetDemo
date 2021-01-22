#include "frmtcpserver.h"
#include "ui_frmtcpserver.h"
#include "quiwidget.h"

frmTcpServer::frmTcpServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmTcpServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initIP();
    this->initConfig();
}

frmTcpServer::~frmTcpServer()
{
    delete ui;
}

void frmTcpServer::initForm()
{
    isOk = false;
    tcpServer = new TcpServer(this);
    connect(tcpServer, SIGNAL(clientConnected(QString, int)), this, SLOT(clientConnected(QString, int)));
    connect(tcpServer, SIGNAL(clientDisconnected(QString, int)), this, SLOT(clientDisconnected(QString, int)));
    connect(tcpServer, SIGNAL(sendData(QString, int, QString)), this, SLOT(sendData(QString, int, QString)));
    connect(tcpServer, SIGNAL(receiveData(QString, int, QString)), this, SLOT(receiveData(QString, int, QString)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(App::Intervals);
    ui->cboxData->addItems(App::Datas);
    QUIHelper::setLabStyle(ui->labCount, 3);
}

void frmTcpServer::initIP()
{
    //获取本机所有IP
    QStringList ips = QUIHelper::getLocalIPs();
    ui->cboxListenIP->addItems(ips);
    ui->cboxListenIP->addItem("127.0.0.1");
}

void frmTcpServer::initConfig()
{
    ui->ckHexSend->setChecked(App::HexSendTcpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(App::HexReceiveTcpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(App::AsciiTcpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(App::DebugTcpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(App::AutoSendTcpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(App::IntervalTcpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(App::TcpListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(App::TcpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(App::SelectAllTcpServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmTcpServer::saveConfig()
{
    App::HexSendTcpServer = ui->ckHexSend->isChecked();
    App::HexReceiveTcpServer = ui->ckHexReceive->isChecked();
    App::AsciiTcpServer = ui->ckAscii->isChecked();
    App::DebugTcpServer = ui->ckDebug->isChecked();
    App::AutoSendTcpServer = ui->ckAutoSend->isChecked();
    App::IntervalTcpServer = ui->cboxInterval->currentText().toInt();
    App::TcpListenIP = ui->cboxListenIP->currentText();
    App::TcpListenPort = ui->txtListenPort->text().trimmed().toInt();
    App::SelectAllTcpServer = ui->ckSelectAll->isChecked();
    App::writeConfig();

    this->changeTimer();
}

void frmTcpServer::changeTimer()
{
    timer->setInterval(App::IntervalTcpServer);
    if (App::AutoSendTcpServer) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmTcpServer::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (ui->ckShow->isChecked()) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain->setTextColor(QColor("darkgreen"));
    } else {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("red"));
    }

    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void frmTcpServer::clientConnected(const QString &ip, int port)
{
    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个客户端").arg(ui->listWidget->count()));
}

void frmTcpServer::clientDisconnected(const QString &ip, int port)
{
    int row = -1;
    QString str = QString("%1:%2").arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->text() == str) {
            row = i;
            break;
        }
    }

    delete ui->listWidget->takeItem(row);
    ui->labCount->setText(QString("共 %1 个客户端").arg(ui->listWidget->count()));
}

void frmTcpServer::sendData(const QString &ip, int port, const QString &data)
{
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
    append(error ? 1 : 0, str);
}

void frmTcpServer::receiveData(const QString &ip, int port, const QString &data)
{
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(1, str);
}

void frmTcpServer::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "监听") {
        isOk = tcpServer->start();
        if (isOk) {
            append(0, "监听成功");
            ui->btnListen->setText("关闭");
        }
    } else {
        isOk = false;
        tcpServer->stop();
        ui->btnListen->setText("监听");
    }
}

void frmTcpServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    if (data.length() <= 0) {
        return;
    }

    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(STRDATETIME);
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }

    on_btnClear_clicked();
}

void frmTcpServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmTcpServer::on_btnSend_clicked()
{
    if (!isOk) {
        return;
    }

    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    if (ui->ckSelectAll->isChecked()) {
        tcpServer->writeData(data);
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            tcpServer->writeData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmTcpServer::on_btnClose_clicked()
{
    if (ui->ckSelectAll->isChecked()) {
        tcpServer->remove();
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            tcpServer->remove(list.at(0), list.at(1).toInt());
        }
    }
}
