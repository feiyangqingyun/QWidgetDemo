#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "webclient.h"

class WebServer : public QWebSocketServer
{
    Q_OBJECT
public:
    explicit WebServer(const QString &serverName = QString(), QWebSocketServer::SslMode secureMode = QWebSocketServer::NonSecureMode, QObject *parent = 0);

private:
    QList<WebClient *> clients;

private slots:
    void newConnection();
    void disconnected();

signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

    void clientConnected(const QString &ip, int port);
    void clientDisconnected(const QString &ip, int port);

public slots:
    //启动服务
    bool start();
    //停止服务
    void stop();

    //指定连接发送数据
    void writeData(const QString &ip, int port, const QString &data);
    //对所有连接发送数据
    void writeData(const QString &data);

    //断开指定连接
    void remove(const QString &ip, int port);
    //断开所有连接
    void remove();
};

#endif // WEBSERVER_H
