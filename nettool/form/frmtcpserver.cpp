#include "frmtcpserver.h"
#include "ui_frmtcpserver.h"
#include "quihelper.h"

frmTcpServer::frmTcpServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmTcpServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    on_btnListen_clicked();
}

frmTcpServer::~frmTcpServer()
{
    delete ui;
}

void frmTcpServer::initForm()
{
    isOk = false;

    //实例化对象并绑定信号槽
    server = new TcpServer(this);
    connect(server, SIGNAL(clientConnected(QString, int)), this, SLOT(clientConnected(QString, int)));
    connect(server, SIGNAL(clientDisconnected(QString, int)), this, SLOT(clientDisconnected(QString, int)));
    connect(server, SIGNAL(sendData(QString, int, QString)), this, SLOT(sendData(QString, int, QString)));
    connect(server, SIGNAL(receiveData(QString, int, QString)), this, SLOT(receiveData(QString, int, QString)));

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
    AppData::loadIP(ui->cboxListenIP);
}

void frmTcpServer::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendTcpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveTcpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiTcpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugTcpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendTcpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalTcpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(AppConfig::TcpListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::TcpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(AppConfig::SelectAllTcpServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmTcpServer::saveConfig()
{
    AppConfig::HexSendTcpServer = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveTcpServer = ui->ckHexReceive->isChecked();
    AppConfig::AsciiTcpServer = ui->ckAscii->isChecked();
    AppConfig::DebugTcpServer = ui->ckDebug->isChecked();
    AppConfig::AutoSendTcpServer = ui->ckAutoSend->isChecked();
    AppConfig::IntervalTcpServer = ui->cboxInterval->currentText().toInt();
    AppConfig::TcpListenIP = ui->cboxListenIP->currentText();
    AppConfig::TcpListenPort = ui->txtListenPort->text().trimmed().toInt();
    AppConfig::SelectAllTcpServer = ui->ckSelectAll->isChecked();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmTcpServer::initTimer()
{
    if (timer->interval() != AppConfig::IntervalTcpServer) {
        timer->setInterval(AppConfig::IntervalTcpServer);
    }

    if (AppConfig::AutoSendTcpServer) {
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
        ui->txtMain->setTextColor(QColor("#22A3A9"));
    } else if (type == 1) {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("#D64D54"));
    } else {
        strType = "信息";
        ui->txtMain->setTextColor(QColor("#A279C5"));
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
        isOk = server->start();
        if (isOk) {
            append(0, "监听成功");
            ui->btnListen->setText("关闭");
        }
    } else {
        isOk = false;
        server->stop();
        ui->btnListen->setText("监听");
    }
}

void frmTcpServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
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
        server->writeData(data);
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            server->writeData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmTcpServer::on_btnClose_clicked()
{
    if (ui->ckSelectAll->isChecked()) {
        server->remove();
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            server->remove(list.at(0), list.at(1).toInt());
        }
    }
}
