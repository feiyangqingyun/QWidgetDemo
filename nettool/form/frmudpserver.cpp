#include "frmudpserver.h"
#include "ui_frmudpserver.h"
#include "quiwidget.h"

frmUdpServer::frmUdpServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmUdpServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmUdpServer::~frmUdpServer()
{
    delete ui;
}

void frmUdpServer::initForm()
{
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

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

void frmUdpServer::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendUdpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveUdpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiUdpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugUdpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendUdpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalUdpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(AppConfig::UdpListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::UdpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(AppConfig::SelectAllUdpServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmUdpServer::saveConfig()
{
    AppConfig::HexSendUdpServer = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveUdpServer = ui->ckHexReceive->isChecked();
    AppConfig::AsciiUdpServer = ui->ckAscii->isChecked();
    AppConfig::DebugUdpServer = ui->ckDebug->isChecked();
    AppConfig::AutoSendUdpServer = ui->ckAutoSend->isChecked();
    AppConfig::IntervalUdpServer = ui->cboxInterval->currentText().toInt();
    AppConfig::UdpListenIP = ui->cboxListenIP->currentText();
    AppConfig::UdpListenPort = ui->txtListenPort->text().trimmed().toInt();
    AppConfig::SelectAllUdpServer = ui->ckSelectAll->isChecked();
    AppConfig::writeConfig();

    this->changeTimer();
}

void frmUdpServer::changeTimer()
{
    timer->setInterval(AppConfig::IntervalUdpServer);
    if (AppConfig::AutoSendUdpServer) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmUdpServer::append(int type, const QString &data, bool clear)
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

void frmUdpServer::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (socket->hasPendingDatagrams()) {
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size(), &host, &port);

        if (AppConfig::HexReceiveUdpServer) {
            buffer = QUIHelper::byteArrayToHexStr(data);
        } else if (AppConfig::AsciiUdpServer) {
            buffer = QUIHelper::byteArrayToAsciiStr(data);
        } else {
            buffer = QString(data);
        }

        QString ip = host.toString();
        ip = ip.replace("::ffff:", "");
        if (ip.isEmpty()) {
            continue;
        }

        QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(buffer);
        append(1, str);
        clientConnected(ip, port);

        if (AppConfig::DebugUdpServer) {
            int count = AppConfig::Keys.count();
            for (int i = 0; i < count; i++) {
                if (AppConfig::Keys.at(i) == buffer) {
                    sendData(ip, port, AppConfig::Values.at(i));
                    break;
                }
            }
        }
    }
}

void frmUdpServer::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendUdpServer) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else if (AppConfig::AsciiUdpServer) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}

void frmUdpServer::clientConnected(const QString &ip, int port)
{
    //先过滤重复的
    QString str = QString("%1:%2").arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QString s = ui->listWidget->item(i)->text();
        if (str == s) {
            return;
        }
    }

    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个客户端").arg(ui->listWidget->count()));
}

void frmUdpServer::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "监听") {
        bool ok = socket->bind(QHostAddress(AppConfig::UdpListenIP), AppConfig::UdpListenPort);
        if (ok) {
            ui->btnListen->setText("关闭");
            append(0, "监听成功");
        }
    } else {
        socket->abort();
        ui->btnListen->setText("监听");
    }
}

void frmUdpServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppConfig::saveData(data);
    on_btnClear_clicked();
}

void frmUdpServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmUdpServer::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    if (ui->ckSelectAll->isChecked()) {
        for (int i = 0; i < ui->listWidget->count(); i++) {
            QString str = ui->listWidget->item(i)->text();
            QStringList list = str.split(":");
            sendData(list.at(0), list.at(1).toInt(), data);
        }
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            sendData(list.at(0), list.at(1).toInt(), data);
        }
    }
}
