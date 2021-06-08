#include "tcpserver.h"
#include "quihelper.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpServer::newConnection()
{
    QTcpSocket *socket = this->nextPendingConnection();
    TcpClient *client = new TcpClient(socket, this);
    connect(client, SIGNAL(clientDisconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(sendData(QString, int, QString)), this, SIGNAL(sendData(QString, int, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString)), this, SIGNAL(receiveData(QString, int, QString)));

    QString ip = client->getIP();
    int port = client->getPort();
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
    bool ok = listen(QHostAddress(AppConfig::TcpListenIP), AppConfig::TcpListenPort);
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
        if (client->getIP() == ip && client->getPort() == port) {
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
        if (client->getIP() == ip && client->getPort() == port) {
            client->abort();
            break;
        }
    }
}

void TcpServer::remove()
{
    foreach (TcpClient *client, clients) {
        client->abort();
    }
}
