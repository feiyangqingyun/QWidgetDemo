#include "frmwebserver.h"
#include "ui_frmwebserver.h"
#include "quiwidget.h"

frmWebServer::frmWebServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmWebServer::~frmWebServer()
{
    delete ui;
}

void frmWebServer::initForm()
{
    isOk = false;
    server = new WebServer("WebServer", QWebSocketServer::NonSecureMode, this);
    connect(server, SIGNAL(clientConnected(QString, int)), this, SLOT(clientConnected(QString, int)));
    connect(server, SIGNAL(clientDisconnected(QString, int)), this, SLOT(clientDisconnected(QString, int)));
    connect(server, SIGNAL(sendData(QString, int, QString)), this, SLOT(sendData(QString, int, QString)));
    connect(server, SIGNAL(receiveData(QString, int, QString)), this, SLOT(receiveData(QString, int, QString)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(AppConfig::Intervals);
    ui->cboxData->addItems(AppConfig::Datas);

    //获取本机所有IP
    QStringList ips = QUIHelper::getLocalIPs();
    ui->cboxListenIP->addItems(ips);
    if (!ips.contains("127.0.0.1")) {
        ui->cboxListenIP->addItem("127.0.0.1");
    }
}

void frmWebServer::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendWebServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveWebServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiWebServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugWebServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendWebServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalWebServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(AppConfig::WebListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::WebListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(AppConfig::SelectAllWebServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmWebServer::saveConfig()
{
    AppConfig::HexSendWebServer = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveWebServer = ui->ckHexReceive->isChecked();
    AppConfig::AsciiWebServer = ui->ckAscii->isChecked();
    AppConfig::DebugWebServer = ui->ckDebug->isChecked();
    AppConfig::AutoSendWebServer = ui->ckAutoSend->isChecked();
    AppConfig::IntervalWebServer = ui->cboxInterval->currentText().toInt();
    AppConfig::WebListenIP = ui->cboxListenIP->currentText();
    AppConfig::WebListenPort = ui->txtListenPort->text().trimmed().toInt();
    AppConfig::SelectAllWebServer = ui->ckSelectAll->isChecked();
    AppConfig::writeConfig();

    this->changeTimer();
}

void frmWebServer::changeTimer()
{
    timer->setInterval(AppConfig::IntervalWebServer);
    if (AppConfig::AutoSendWebServer) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmWebServer::append(int type, const QString &data, bool clear)
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

void frmWebServer::clientConnected(const QString &ip, int port)
{
    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个客户端").arg(ui->listWidget->count()));
}

void frmWebServer::clientDisconnected(const QString &ip, int port)
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

void frmWebServer::sendData(const QString &ip, int port, const QString &data)
{
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
    append(error ? 1 : 0, str);
}

void frmWebServer::receiveData(const QString &ip, int port, const QString &data)
{
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(1, str);
}

void frmWebServer::on_btnListen_clicked()
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

void frmWebServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppConfig::saveData(data);
    on_btnClear_clicked();
}

void frmWebServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmWebServer::on_btnSend_clicked()
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

void frmWebServer::on_btnClose_clicked()
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
