#include "tcpserver2.h"
#include "quihelper.h"
#include "quihelperdata.h"

TcpClient2::TcpClient2(QObject *parent) :  QTcpSocket(parent)
{
    ip = "127.0.0.1";
    port = 6908;
    deviceID = "SSJC00000001";

    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(this, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
#else
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
#endif
}

void TcpClient2::setIP(const QString &ip)
{
    this->ip = ip;
}

QString TcpClient2::getIP() const
{
    return this->ip;
}

void TcpClient2::setPort(int port)
{
    this->port = port;
}

int TcpClient2::getPort() const
{
    return this->port;
}

QString TcpClient2::getDeviceID()
{
    return this->deviceID;
}

void TcpClient2::readData()
{
    QByteArray data = this->readAll();
    if (data.length() <= 0) {
        return;
    }

    //取出唯一标识符,并过滤,可自行更改过滤条件
    QByteArray cmd = data.mid(AppConfig::CmdStart2, AppConfig::CmdLen2);
    QString id = QString(cmd);
    if (id.startsWith("S") && deviceID != id) {
        deviceID = id;
        //发送信号更新标识符
        emit receiveDeviceID(ip, port, deviceID);
    }

    QString buffer;
    if (AppConfig::HexData2) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
        buffer = QString(data);
    }

    emit receiveData(ip, port, deviceID, buffer);
}

void TcpClient2::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexData2) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }

    this->write(buffer);
    emit sendData(ip, port, deviceID, data);
}

TcpServer2::TcpServer2(QObject *parent) : QTcpServer(parent)
{
}

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
void TcpServer2::incomingConnection(qintptr handle)
#else
void TcpServer2::incomingConnection(int handle)
#endif
{
    TcpClient2 *client = new TcpClient2(this);
    client->setSocketDescriptor(handle);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(sendData(QString, int, QString, QString)), this, SIGNAL(sendData(QString, int, QString, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString, QString)), this, SIGNAL(receiveData(QString, int, QString, QString)));
    connect(client, SIGNAL(receiveDeviceID(QString, int, QString)), this, SIGNAL(receiveDeviceID(QString, int, QString)));

    QString ip = client->peerAddress().toString();
    int port = client->peerPort();
    QString deviceID = client->getDeviceID();
    client->setIP(ip);
    client->setPort(port);
    emit clientConnected(ip, port, deviceID);
    emit sendData(ip, port, deviceID, "客户端上线");

    //追加到链表中
    clients.append(client);
}

void TcpServer2::disconnected()
{
    TcpClient2 *client = (TcpClient2 *)sender();
    QString ip = client->getIP();
    int port = client->getPort();
    QString deviceID = client->getDeviceID();
    emit clientDisconnected(ip, port, deviceID);
    emit sendData(ip, port, deviceID, "客户端下线");

    //断开连接后从链表中移除
    clients.removeOne(client);
}

bool TcpServer2::start()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    bool ok = listen(QHostAddress::AnyIPv4, AppConfig::ListenPort2);
#else
    bool ok = listen(QHostAddress::Any, AppConfig::ListenPort2);
#endif
    return ok;
}

void TcpServer2::stop()
{
    foreach (TcpClient2 *client, clients) {
        client->disconnectFromHost();
    }

    this->close();
}

bool TcpServer2::writeData(const QString &deviceID, const QString &data)
{
    bool ok = false;
    foreach (TcpClient2 *client, clients) {
        if (client->getDeviceID() == deviceID) {
            client->sendData(data);            
            ok = true;            
        }
    }   

    return ok;
}
