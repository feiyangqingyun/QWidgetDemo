#include "frmtcpclient.h"
#include "ui_frmtcpclient.h"
#include "quiwidget.h"

frmTcpClient::frmTcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmTcpClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmTcpClient::~frmTcpClient()
{
    delete ui;
}

void frmTcpClient::initForm()
{
    isOk = false;
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(disconnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(App::Intervals);
    ui->cboxData->addItems(App::Datas);

#ifndef emsdk
    QString ip = QUIHelper::getNetIP(QUIHelper::getHtml("http://whois.pconline.com.cn/"));
    append(1, QString("外网IP -> %1").arg(ip));
#endif
}

void frmTcpClient::initConfig()
{
    ui->ckHexSend->setChecked(App::HexSendTcpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(App::HexReceiveTcpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(App::AsciiTcpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(App::DebugTcpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(App::AutoSendTcpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(App::IntervalTcpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(App::TcpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(App::TcpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmTcpClient::saveConfig()
{
    App::HexSendTcpClient = ui->ckHexSend->isChecked();
    App::HexReceiveTcpClient = ui->ckHexReceive->isChecked();
    App::AsciiTcpClient = ui->ckAscii->isChecked();
    App::DebugTcpClient = ui->ckDebug->isChecked();
    App::AutoSendTcpClient = ui->ckAutoSend->isChecked();
    App::IntervalTcpClient = ui->cboxInterval->currentText().toInt();
    App::TcpServerIP = ui->txtServerIP->text().trimmed();
    App::TcpServerPort = ui->txtServerPort->text().trimmed().toInt();
    App::writeConfig();

    this->changeTimer();
}

void frmTcpClient::changeTimer()
{
    timer->setInterval(App::IntervalTcpClient);
    if (App::AutoSendTcpClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmTcpClient::append(int type, const QString &data, bool clear)
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

void frmTcpClient::connected()
{
    isOk = true;
    ui->btnConnect->setText("断开");
    append(0, "服务器连接");
}

void frmTcpClient::disconnected()
{
    isOk = false;
    socket->abort();
    ui->btnConnect->setText("连接");
    append(1, "服务器断开");
}

void frmTcpClient::readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (App::HexReceiveTcpClient) {
        buffer = QUIHelper::byteArrayToHexStr(data);
    } else if (App::AsciiTcpClient) {
        buffer = QUIHelper::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    append(1, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (App::DebugTcpClient) {
        int count = App::Keys.count();
        for (int i = 0; i < count; i++) {
            if (App::Keys.at(i) == buffer) {
                sendData(App::Values.at(i));
                break;
            }
        }
    }
}

void frmTcpClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (App::HexSendTcpClient) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else if (App::AsciiTcpClient) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->write(buffer);
    append(0, data);
}

void frmTcpClient::on_btnConnect_clicked()
{
    if (ui->btnConnect->text() == "连接") {
        socket->abort();
        socket->connectToHost(App::TcpServerIP, App::TcpServerPort);
    } else {
        socket->abort();
    }
}

void frmTcpClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    App::saveData(data);
    on_btnClear_clicked();
}

void frmTcpClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmTcpClient::on_btnSend_clicked()
{
    if (!isOk) {
        return;
    }

    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(data);
}
