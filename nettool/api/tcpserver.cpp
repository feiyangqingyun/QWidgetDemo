#include "tcpserver.h"
#include "quiwidget.h"

TcpClient::TcpClient(QObject *parent) :  QTcpSocket(parent)
{
    ip = "127.0.0.1";
    port = 6000;

    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void TcpClient::setIP(const QString &ip)
{
    this->ip = ip;
}

QString TcpClient::getIP() const
{
    return this->ip;
}

void TcpClient::setPort(int port)
{
    this->port = port;
}

int TcpClient::getPort() const
{
    return this->port;
}

void TcpClient::readData()
{
    QByteArray data = this->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (App::HexReceiveTcpServer) {
        buffer = QUIHelper::byteArrayToHexStr(data);
    } else if (App::AsciiTcpServer) {
        buffer = QUIHelper::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    emit receiveData(ip, port, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (App::DebugTcpServer) {
        int count = App::Keys.count();
        for (int i = 0; i < count; i++) {
            if (App::Keys.at(i) == buffer) {
                sendData(App::Values.at(i));
                break;
            }
        }
    }
}

void TcpClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (App::HexSendTcpServer) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else if (App::AsciiTcpServer) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }

    this->write(buffer);
    emit sendData(ip, port, data);
}

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
}

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
void TcpServer::incomingConnection(qintptr handle)
#else
void TcpServer::incomingConnection(int handle)
#endif
{
    TcpClient *client = new TcpClient(this);
    client->setSocketDescriptor(handle);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(sendData(QString, int, QString)), this, SIGNAL(sendData(QString, int, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString)), this, SIGNAL(receiveData(QString, int, QString)));

    QString ip = client->peerAddress().toString();
    ip = ip.replace("::ffff:", "");
    int port = client->peerPort();
    client->setIP(ip);
    client->setPort(port);
    emit clientConnected(ip, port);
    emit sendData(ip, port, "客户端上线");

    //连接后加入链表
    clients.append(client);
}

void TcpServer::disconnected()
{
    TcpClient *client = (TcpClient *)sender();
    QString ip = client->getIP();
    int port = client->getPort();
    emit clientDisconnected(ip, port);
    emit sendData(ip, port, "客户端下线");

    //断开连接后从链表中移除
    clients.removeOne(client);
}

bool TcpServer::start()
{
    bool ok = listen(QHostAddress(App::TcpListenIP), App::TcpListenPort);
    return ok;
}

void TcpServer::stop()
{
    remove();
    this->close();
}

void TcpServer::writeData(const QString &ip, int port, const QString &data)
{
    foreach (TcpClient *client, clients) {
        if (client->peerAddress().toString() == ip && client->peerPort() == port) {
            client->sendData(data);
            break;
        }
    }
}

void TcpServer::writeData(const QString &data)
{
    foreach (TcpClient *client, clients) {
        client->sendData(data);
    }
}

void TcpServer::remove(const QString &ip, int port)
{
    foreach (TcpClient *client, clients) {
        if (client->peerAddress().toString() == ip && client->peerPort() == port) {
            client->disconnectFromHost();
            break;
        }
    }
}

void TcpServer::remove()
{
    foreach (TcpClient *client, clients) {
        client->disconnectFromHost();
    }
}
