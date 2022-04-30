#include "frmtcpclient.h"
#include "ui_frmtcpclient.h"
#include "quihelper.h"
#include "quihelperdata.h"

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

bool frmTcpClient::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmTcpClient::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
    AppData::loadIP(ui->cboxBindIP);    
}

void frmTcpClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendTcpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveTcpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiTcpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugTcpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendTcpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalTcpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxBindIP->setCurrentIndex(ui->cboxBindIP->findText(AppConfig::TcpBindIP));
    connect(ui->cboxBindIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtBindPort->setText(QString::number(AppConfig::TcpBindPort));
    connect(ui->txtBindPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::TcpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::TcpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmTcpClient::saveConfig()
{
    AppConfig::HexSendTcpClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveTcpClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiTcpClient = ui->ckAscii->isChecked();
    AppConfig::DebugTcpClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendTcpClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalTcpClient = ui->cboxInterval->currentText().toInt();
    AppConfig::TcpBindIP = ui->cboxBindIP->currentText();
    AppConfig::TcpBindPort = ui->txtBindPort->text().trimmed().toInt();
    AppConfig::TcpServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::TcpServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmTcpClient::initTimer()
{
    if (timer->interval() != AppConfig::IntervalTcpClient) {
        timer->setInterval(AppConfig::IntervalTcpClient);
    }

    if (AppConfig::AutoSendTcpClient) {
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

void frmTcpClient::connected()
{
    isOk = true;
    ui->btnConnect->setText("断开");
    append(0, "服务器连接");
    append(0, QString("本地地址: %1  本地端口: %2").arg(socket->localAddress().toString()).arg(socket->localPort()));
    append(0, QString("远程地址: %1  远程端口: %2").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void frmTcpClient::disconnected()
{
    isOk = false;
    ui->btnConnect->setText("连接");
    append(1, "服务器断开");
}

void frmTcpClient::error()
{
    append(2, socket->errorString());
}

void frmTcpClient::readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (AppConfig::HexReceiveTcpClient) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else if (AppConfig::AsciiTcpClient) {
        buffer = QUIHelperData::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    append(1, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugTcpClient) {
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void frmTcpClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendTcpClient) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiTcpClient) {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->write(buffer);
    append(0, data);
}

void frmTcpClient::on_btnConnect_clicked()
{
    if (ui->btnConnect->text() == "连接") {
        //断开所有连接和操作
        //socket->abort();
        //绑定网卡和端口
        //有个后遗症,客户端这边断开连接后还会保持几分钟导致不能重复绑定
        //如果是服务器断开则可以继续使用
        //提示 The bound address is already in use
        //参考 https://www.cnblogs.com/baiduboy/p/7426822.html
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        //socket->bind(QHostAddress(AppConfig::TcpBindIP), AppConfig::TcpBindPort);
#endif
        //连接服务器
        socket->connectToHost(AppConfig::TcpServerIP, AppConfig::TcpServerPort);
    } else {
        socket->abort();
    }
}

void frmTcpClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
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
