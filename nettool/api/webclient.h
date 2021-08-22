#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <QtWebSockets>

//为了方便并发和单独处理数据所以单独写个类专门处理
#if 1
//小数据量可以不用线程,收发数据本身默认异步的
class WebClient : public QObject
#else
//线程方便处理密集运算比如解析图片
class WebClient : public QThread
#endif
{
    Q_OBJECT
public:
    explicit WebClient(QWebSocket *socket, QObject *parent = 0);

private:
    QWebSocket *socket;
    QString ip;
    int port;

public:
    QString getIP() const;
    int getPort()   const;

private slots:
    void slot_disconnected();
    void slot_error();

    void textFrameReceived(const QString &data, bool isLastFrame);
    void binaryFrameReceived(const QByteArray &data, bool isLastFrame);
    void textMessageReceived(const QString &data);
    void binaryMessageReceived(const QByteArray &data);

signals:
    void disconnected(const QString &ip, int port);
    void error(const QString &ip, int port, const QString &error);

    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

public slots:
    void sendData(const QString &data);
    void abort();
};

#endif // WEBCLIENT_H
