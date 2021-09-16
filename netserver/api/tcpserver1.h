#ifndef TCPSERVER1_H
#define TCPSERVER1_H

#include <QtNetwork>

class TcpClient1 : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient1(QObject *parent = 0);

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

class TcpServer1 : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer1(QObject *parent = 0);

private:
    QList<TcpClient1 *> clients;

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

#endif // TCPSERVER1_H
