#include "frmudpclient.h"
#include "ui_frmudpclient.h"
#include "quiwidget.h"

frmUdpClient::frmUdpClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmUdpClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmUdpClient::~frmUdpClient()
{
    delete ui;
}

void frmUdpClient::initForm()
{
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    ui->cboxInterval->addItems(AppConfig::Intervals);
    ui->cboxData->addItems(AppConfig::Datas);
}

void frmUdpClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendUdpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveUdpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiUdpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugUdpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendUdpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalUdpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::UdpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::UdpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->changeTimer();
}

void frmUdpClient::saveConfig()
{
    AppConfig::HexSendUdpClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveUdpClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiUdpClient = ui->ckAscii->isChecked();
    AppConfig::DebugUdpClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendUdpClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalUdpClient = ui->cboxInterval->currentText().toInt();
    AppConfig::UdpServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::UdpServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->changeTimer();
}

void frmUdpClient::changeTimer()
{
    timer->setInterval(AppConfig::IntervalUdpClient);
    if (AppConfig::AutoSendUdpClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmUdpClient::append(int type, const QString &data, bool clear)
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

void frmUdpClient::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (socket->hasPendingDatagrams()) {
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size(), &host, &port);

        if (AppConfig::HexReceiveUdpClient) {
            buffer = QUIHelper::byteArrayToHexStr(data);
        } else if (AppConfig::AsciiUdpClient) {
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

        if (AppConfig::DebugUdpClient) {
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

void frmUdpClient::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendUdpClient) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else if (AppConfig::AsciiUdpClient) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}

void frmUdpClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppConfig::saveData(data);
    on_btnClear_clicked();
}

void frmUdpClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmUdpClient::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}
