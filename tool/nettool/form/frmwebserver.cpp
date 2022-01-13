#include "frmwebserver.h"
#include "ui_frmwebserver.h"
#include "quihelper.h"

frmWebServer::frmWebServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    on_btnListen_clicked();
}

frmWebServer::~frmWebServer()
{
    delete ui;
}

bool frmWebServer::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmWebServer::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    server = new WebServer("WebServer", QWebSocketServer::NonSecureMode, this);
    connect(server, SIGNAL(connected(QString, int)), this, SLOT(connected(QString, int)));
    connect(server, SIGNAL(disconnected(QString, int)), this, SLOT(disconnected(QString, int)));
    connect(server, SIGNAL(error(QString, int, QString)), this, SLOT(error(QString, int, QString)));
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

    this->initTimer();
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

    this->initTimer();
}

void frmWebServer::initTimer()
{
    if (timer->interval() != AppConfig::IntervalWebServer) {
        timer->setInterval(AppConfig::IntervalWebServer);
    }

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

void frmWebServer::connected(const QString &ip, int port)
{
    append(0, QString("[%1:%2] %3").arg(ip).arg(port).arg("客户端上线"));

    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个客户端").arg(ui->listWidget->count()));
}

void frmWebServer::disconnected(const QString &ip, int port)
{
    append(2, QString("[%1:%2] %3").arg(ip).arg(port).arg("客户端下线"));

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

void frmWebServer::error(const QString &ip, int port, const QString &error)
{
    append(2, QString("[%1:%2] %3").arg(ip).arg(port).arg(error));
}

void frmWebServer::sendData(const QString &ip, int port, const QString &data)
{
    append(0, QString("[%1:%2] %3").arg(ip).arg(port).arg(data));
}

void frmWebServer::receiveData(const QString &ip, int port, const QString &data)
{
    append(1, QString("[%1:%2] %3").arg(ip).arg(port).arg(data));
}

void frmWebServer::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "监听") {
        isOk = server->start();
        if (isOk) {
            append(0, "监听成功");
            ui->btnListen->setText("关闭");
        } else {
            append(2, QString("监听失败: %1").arg(server->errorString()));
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
    AppData::saveData(data);
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

void frmWebServer::on_btnRemove_clicked()
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
