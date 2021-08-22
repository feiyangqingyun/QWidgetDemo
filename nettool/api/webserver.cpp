#include "webserver.h"
#include "quihelper.h"

WebServer::WebServer(const QString &serverName, SslMode secureMode, QObject *parent) : QWebSocketServer(serverName, secureMode, parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(slot_newConnection()));
}

void WebServer::slot_newConnection()
{
    QWebSocket *socket = this->nextPendingConnection();
    WebClient *client = new WebClient(socket, this);
    connect(client, SIGNAL(disconnected(QString, int)), this, SLOT(slot_disconnected(QString, int)));
    connect(client, SIGNAL(error(QString, int, QString)), this, SIGNAL(error(QString, int, QString)));
    connect(client, SIGNAL(sendData(QString, int, QString)), this, SIGNAL(sendData(QString, int, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString)), this, SIGNAL(receiveData(QString, int, QString)));

    emit connected(client->getIP(), client->getPort());
    //连接后加入链表
    clients.append(client);
}

void WebServer::slot_disconnected(const QString &ip, int port)
{
    WebClient *client = (WebClient *)sender();
    emit disconnected(ip, port);
    //断开连接后从链表中移除
    clients.removeOne(client);
}

bool WebServer::start()
{
    bool ok = listen(QHostAddress(AppConfig::WebListenIP), AppConfig::WebListenPort);
    return ok;
}

void WebServer::stop()
{
    remove();
    this->close();
}

void WebServer::writeData(const QString &ip, int port, const QString &data)
{
    foreach (WebClient *client, clients) {
        if (client->getIP() == ip && client->getPort() == port) {
            client->sendData(data);
            break;
        }
    }
}

void WebServer::writeData(const QString &data)
{
    foreach (WebClient *client, clients) {
        client->sendData(data);
    }
}

void WebServer::remove(const QString &ip, int port)
{
    foreach (WebClient *client, clients) {
        if (client->getIP() == ip && client->getPort() == port) {
            client->abort();
            break;
        }
    }
}

void WebServer::remove()
{
    foreach (WebClient *client, clients) {
        client->abort();
    }
}
