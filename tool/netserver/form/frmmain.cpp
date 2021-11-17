#include "frmmain.h"
#include "ui_frmmain.h"
#include "quihelper.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    on_btnListen1_clicked();
    on_btnListen2_clicked();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    tcpServer1 = new TcpServer1(this);
    connect(tcpServer1, SIGNAL(clientConnected(QString, int, QString)), this, SLOT(clientConnected1(QString, int, QString)));
    connect(tcpServer1, SIGNAL(clientDisconnected(QString, int, QString)), this, SLOT(clientDisconnected1(QString, int, QString)));
    connect(tcpServer1, SIGNAL(sendData(QString, int, QString, QString)), this, SLOT(sendData1(QString, int, QString, QString)));
    connect(tcpServer1, SIGNAL(receiveData(QString, int, QString, QString)), this, SLOT(receiveData1(QString, int, QString, QString)));
    connect(tcpServer1, SIGNAL(receiveDeviceID(QString, int, QString)), this, SLOT(receiveDeviceID1(QString, int, QString)));

    tcpServer2 = new TcpServer2(this);
    connect(tcpServer2, SIGNAL(clientConnected(QString, int, QString)), this, SLOT(clientConnected2(QString, int, QString)));
    connect(tcpServer2, SIGNAL(clientDisconnected(QString, int, QString)), this, SLOT(clientDisconnected2(QString, int, QString)));
    connect(tcpServer2, SIGNAL(sendData(QString, int, QString, QString)), this, SLOT(sendData2(QString, int, QString, QString)));
    connect(tcpServer2, SIGNAL(receiveData(QString, int, QString, QString)), this, SLOT(receiveData2(QString, int, QString, QString)));
    connect(tcpServer2, SIGNAL(receiveDeviceID(QString, int, QString)), this, SLOT(receiveDeviceID2(QString, int, QString)));
}

void frmMain::initConfig()
{
    ui->txtListenPort1->setText(QString::number(AppConfig::ListenPort1));
    connect(ui->txtListenPort1, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort2->setText(QString::number(AppConfig::ListenPort2));
    connect(ui->txtListenPort2, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmMain::saveConfig()
{
    AppConfig::ListenPort1 = ui->txtListenPort1->text().trimmed().toInt();
    AppConfig::ListenPort2 = ui->txtListenPort2->text().trimmed().toInt();
    AppConfig::writeConfig();
}

void frmMain::append1(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain1->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain1->clear();
        currentCount = 0;
    }

    //过滤回车换行符
    QString strData = data.left(500);
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain1->setTextColor(QColor("darkgreen"));
    } else {
        strType = "接收";
        ui->txtMain1->setTextColor(QColor("red"));
    }

    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain1->append(strData);
    currentCount++;
}

void frmMain::append2(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain2->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain2->clear();
        currentCount = 0;
    }

    //过滤回车换行符
    QString strData = data.left(500);
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain2->setTextColor(QColor("darkgreen"));
    } else {
        strType = "接收";
        ui->txtMain2->setTextColor(QColor("red"));
    }

    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain2->append(strData);
    currentCount++;
}

void frmMain::clientConnected1(const QString &ip, int port, const QString &deviceID)
{
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    ui->listWidget1->addItem(str);
    ui->labCount1->setText(QString("共 %1 个连接").arg(ui->listWidget1->count()));
}

void frmMain::clientDisconnected1(const QString &ip, int port, const QString &deviceID)
{
    int row = -1;
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget1->count(); i++) {
        if (ui->listWidget1->item(i)->text() == str) {
            row = i;
            break;
        }
    }

    ui->listWidget1->takeItem(row);
    ui->labCount1->setText(QString("共 %1 个连接").arg(ui->listWidget1->count()));
}

void frmMain::sendData1(const QString &ip, int port, const QString &deviceID, const QString &data)
{
    QString str = QString("%1 [%2:%3] %4").arg(deviceID).arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
    append1(error ? 1 : 0, str);
}

void frmMain::receiveData1(const QString &ip, int port, const QString &deviceID, const QString &data)
{
    QString str = QString("%1 [%2:%3] %4").arg(deviceID).arg(ip).arg(port).arg(data);
    append1(1, str);

    //将收到的数据转发到另一路网络
    bool ok = tcpServer2->writeData(deviceID, data);
    sendData2(ip, port, deviceID, ok ? "转发成功" : "对方离线");
    if (!ok) {
        tcpServer1->writeData(deviceID, "deviceError");
    }
}

void frmMain::receiveDeviceID1(const QString &ip, int port, const QString &deviceID)
{
    QString temp = QString("%1:%2").arg(ip).arg(port);
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget1->count(); i++) {
        if (ui->listWidget1->item(i)->text().endsWith(temp)) {
            ui->listWidget1->item(i)->setText(str);
            break;
        }
    }
}

void frmMain::clientConnected2(const QString &ip, int port, const QString &deviceID)
{
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    ui->listWidget2->addItem(str);
    ui->labCount2->setText(QString("共 %1 个连接").arg(ui->listWidget2->count()));
}

void frmMain::clientDisconnected2(const QString &ip, int port, const QString &deviceID)
{
    int row = -1;
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget2->count(); i++) {
        if (ui->listWidget2->item(i)->text() == str) {
            row = i;
            break;
        }
    }

    ui->listWidget2->takeItem(row);
    ui->labCount2->setText(QString("共 %1 个连接").arg(ui->listWidget2->count()));
}

void frmMain::sendData2(const QString &ip, int port, const QString &deviceID, const QString &data)
{
    QString str = QString("%1 [%2:%3] %4").arg(deviceID).arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
    append2(error ? 1 : 0, str);
}

void frmMain::receiveData2(const QString &ip, int port, const QString &deviceID, const QString &data)
{
    QString str = QString("%1 [%2:%3] %4").arg(deviceID).arg(ip).arg(port).arg(data);
    append2(1, str);

    //将收到的数据转发到另一路网络
    bool ok = tcpServer1->writeData(deviceID, data);
    sendData1(ip, port, deviceID, ok ? "转发成功" : "对方离线");
    if (!ok) {
        tcpServer2->writeData(deviceID, "deviceError");
    }
}

void frmMain::receiveDeviceID2(const QString &ip, int port, const QString &deviceID)
{
    QString temp = QString("%1:%2").arg(ip).arg(port);
    QString str = QString("%1 %2:%3").arg(deviceID).arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget2->count(); i++) {
        if (ui->listWidget2->item(i)->text().endsWith(temp)) {
            ui->listWidget2->item(i)->setText(str);
            break;
        }
    }
}

void frmMain::on_btnListen1_clicked()
{
    if (ui->btnListen1->text() == "监听") {
        if (tcpServer1->start()) {
            ui->btnListen1->setText("关闭");
        }
    } else {
        tcpServer1->stop();
        ui->btnListen1->setText("监听");
    }
}

void frmMain::on_btnClear1_clicked()
{
    append1(0, "", true);
}

void frmMain::on_btnListen2_clicked()
{
    if (ui->btnListen2->text() == "监听") {
        if (tcpServer2->start()) {
            ui->btnListen2->setText("关闭");
        }
    } else {
        tcpServer2->stop();
        ui->btnListen2->setText("监听");
    }
}

void frmMain::on_btnClear2_clicked()
{
    append2(0, "", true);
}
