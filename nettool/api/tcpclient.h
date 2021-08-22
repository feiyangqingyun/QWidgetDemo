#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QtNetwork>

//为了方便并发和单独处理数据所以单独写个类专门处理
#if 1
//小数据量可以不用线程,收发数据本身默认异步的
class TcpClient : public QObject
#else
//线程方便处理密集运算比如解析图片
class TcpClient : public QThread
#endif
{
    Q_OBJECT
public:
    explicit TcpClient(QTcpSocket *socket, QObject *parent = 0);

private:
    QTcpSocket *socket;
    QString ip;
    int port;

public:
    QString getIP() const;
    int getPort()   const;

private slots:
    void slot_disconnected();
    void slot_error();
    void slot_readData();

signals:
    void disconnected(const QString &ip, int port);
    void error(const QString &ip, int port, const QString &error);

    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

public slots:
    void sendData(const QString &data);
    void disconnectFromHost();
    void abort();
};

#endif // TCPCLIENT_H
