#include "frmwebclient.h"
#include "ui_frmwebclient.h"
#include "quihelper.h"

frmWebClient::frmWebClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmWebClient::~frmWebClient()
{
    delete ui;
}

void frmWebClient::initForm()
{
    isOk = false;
    socket = new QWebSocket("WebSocket", QWebSocketProtocol::VersionLatest, this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(disconnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    //暂时使用前面两个信号,部分系统上后面两个信号Qt没实现,目前测试到5.15.2
    //在win上如果两组信号都关联了则都会触发,另外一组信号就是多个参数表示是否是最后一个数据包
    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    //connect(socket, SIGNAL(textFrameReceived(QString, bool)), this, SLOT(textFrameReceived(QString, bool)));
    //connect(socket, SIGNAL(binaryFrameReceived(QByteArray, bool)), this, SLOT(binaryFrameReceived(QByteArray, bool)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(AppConfig::Intervals);
    ui->cboxData->addItems(AppConfig::Datas);    
}

void frmWebClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendWebClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveWebClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiWebClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugWebClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendWebClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalWebClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::WebServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::WebServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmWebClient::saveConfig()
{
    AppConfig::HexSendWebClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveWebClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiWebClient = ui->ckAscii->isChecked();
    AppConfig::DebugWebClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendWebClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalWebClient = ui->cboxInterval->currentText().toInt();
    AppConfig::WebServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::WebServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->changeTimer();
}

void frmWebClient::changeTimer()
{
    timer->setInterval(AppConfig::IntervalWebClient);
    if (AppConfig::AutoSendWebClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmWebClient::append(int type, const QString &data, bool clear)
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

void frmWebClient::connected()
{
    isOk = true;
    ui->btnConnect->setText("断开");
    append(0, "服务器连接");
}

void frmWebClient::disconnected()
{
    isOk = false;
    socket->abort();
    ui->btnConnect->setText("连接");
    append(1, "服务器断开");
}

void frmWebClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendWebClient) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    if (AppConfig::AsciiWebClient) {
        socket->sendTextMessage(data);
    } else {
        socket->sendBinaryMessage(buffer);
    }

    append(0, data);
}

void frmWebClient::textFrameReceived(const QString &data, bool isLastFrame)
{
    QString buffer = data;
    append(1, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugWebClient) {
        int count = AppConfig::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppConfig::Keys.at(i) == buffer) {
                sendData(AppConfig::Values.at(i));
                break;
            }
        }
    }
}

void frmWebClient::binaryFrameReceived(const QByteArray &data, bool isLastFrame)
{
    QString buffer;
    if (AppConfig::HexReceiveWebClient) {
        buffer = QUIHelper::byteArrayToHexStr(data);
    } else {
        buffer = QString(data);
    }

    textFrameReceived(buffer, isLastFrame);
}

void frmWebClient::textMessageReceived(const QString &data)
{
    textFrameReceived(data, true);
}

void frmWebClient::binaryMessageReceived(const QByteArray &data)
{
    binaryFrameReceived(data, true);
}

void frmWebClient::on_btnConnect_clicked()
{
    if (ui->btnConnect->text() == "连接") {
        QString url = QString("%1:%2").arg(AppConfig::WebServerIP).arg(AppConfig::WebServerPort);
        socket->abort();
        socket->open(QUrl(url));
    } else {
        socket->abort();
    }
}

void frmWebClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppConfig::saveData(data);
    on_btnClear_clicked();
}

void frmWebClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmWebClient::on_btnSend_clicked()
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
