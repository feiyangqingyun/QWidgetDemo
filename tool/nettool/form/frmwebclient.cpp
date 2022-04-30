#include "frmwebclient.h"
#include "ui_frmwebclient.h"
#include "quihelper.h"
#include "quihelperdata.h"

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

bool frmWebClient::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmWebClient::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    socket = new QWebSocket("WebSocket", QWebSocketProtocol::VersionLatest, this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

    //暂时使用前面两个信号,部分系统上后面两个信号Qt没实现,目前测试到5.15.2
    //在win上如果两组信号都关联了则都会触发,另外一组信号就是多个参数表示是否是最后一个数据包
    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    //connect(socket, SIGNAL(textFrameReceived(QString, bool)), this, SLOT(textFrameReceived(QString, bool)));
    //connect(socket, SIGNAL(binaryFrameReceived(QByteArray, bool)), this, SLOT(binaryFrameReceived(QByteArray, bool)));

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
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

    this->initTimer();
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

    this->initTimer();
}

void frmWebClient::initTimer()
{
    if (timer->interval() != AppConfig::IntervalWebClient) {
        timer->setInterval(AppConfig::IntervalWebClient);
    }

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
        ui->txtMain->setTextColor(QColor("#22A3A9"));
    } else if (type == 1) {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("#753775"));
    } else {
        strType = "错误";
        ui->txtMain->setTextColor(QColor("#D64D54"));
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
    append(0, QString("本地地址: %1  本地端口: %2").arg(socket->localAddress().toString()).arg(socket->localPort()));
    append(0, QString("远程地址: %1  远程端口: %2").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void frmWebClient::disconnected()
{
    isOk = false;
    ui->btnConnect->setText("连接");
    append(1, "服务器断开");
}

void frmWebClient::error()
{
    append(2, socket->errorString());
}

void frmWebClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendWebClient) {
        buffer = QUIHelperData::hexStrToByteArray(data);
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
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void frmWebClient::binaryFrameReceived(const QByteArray &data, bool isLastFrame)
{
    QString buffer;
    if (AppConfig::HexReceiveWebClient) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
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
    AppData::saveData(data);
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
