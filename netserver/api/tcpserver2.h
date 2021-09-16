#ifndef TCPSERVER2_H
#define TCPSERVER2_H

#include <QtNetwork>

class TcpClient2 : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient2(QObject *parent = 0);

private:
    QString ip;
    int port;
    QString deviceID;

public:
    void setIP(const QString &ip);
    QString getIP()const;
    void setPort(int port);
    int getPort()const;

    QString getDeviceID();

private slots:
    void readData();

signals:
    void sendData(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveData(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveDeviceID(const QString &ip, int port, const QString &deviceID);

public slots:
    void sendData(const QString &data);

};

class TcpServer2 : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer2(QObject *parent = 0);

private:
    QList<TcpClient2 *> clients;

protected:
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    void incomingConnection(qintptr handle);
#else
    void incomingConnection(int handle);
#endif

private slots:
    void disconnected();

signals:
    void clientConnected(const QString &ip, int port, const QString &deviceID);
    void clientDisconnected(const QString &ip, int port, const QString &deviceID);
    void sendData(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveData(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveDeviceID(const QString &ip, int port, const QString &deviceID);

public slots:
    //启动服务
    bool start();
    //停止服务
    void stop();

    //对指定连接发送数据
    bool writeData(const QString &deviceID, const QString &data);
};

#endif // TCPSERVER2_H
