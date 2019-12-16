#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QtNetwork>

class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

private:
    QString ip;
    int port;

public:
    void setIP(const QString &ip);
    QString getIP()const;

    void setPort(int port);
    int getPort()const;

private slots:
    void readData();

signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

public slots:
    void sendData(const QString &data);

};

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

private:
    QList<TcpClient *> clients;

protected:
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    void incomingConnection(qintptr handle);
#else
    void incomingConnection(int handle);
#endif

private slots:
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

#endif // TCPSERVER_H
