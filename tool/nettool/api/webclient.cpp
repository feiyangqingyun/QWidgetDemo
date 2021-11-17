#include "webclient.h"
#include "quihelper.h"
#include "quihelperdata.h"

WebClient::WebClient(QWebSocket *socket, QObject *parent) : QObject(parent)
{
    this->socket = socket;
    ip = socket->peerAddress().toString();
    ip = ip.replace("::ffff:", "");
    port = socket->peerPort();

    connect(socket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error()));

    //暂时使用前面两个信号,部分系统后面两个信号Qt没实现,目前测试到5.15.2
    //在win上如果两组信号都关联了则都会触发,另外一组信号就是多个参数表示是否是最后一个数据包
    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    //connect(socket, SIGNAL(textFrameReceived(QString, bool)), this, SLOT(textFrameReceived(QString, bool)));
    //connect(socket, SIGNAL(binaryFrameReceived(QByteArray, bool)), this, SLOT(binaryFrameReceived(QByteArray, bool)));
}

QString WebClient::getIP() const
{
    return this->ip;
}

int WebClient::getPort() const
{
    return this->port;
}

void WebClient::slot_disconnected()
{
    emit disconnected(ip, port);
    socket->deleteLater();
    this->deleteLater();
}

void WebClient::slot_error()
{
    emit error(ip, port, socket->errorString());
}

void WebClient::textFrameReceived(const QString &data, bool isLastFrame)
{
    QString buffer = data;
    emit receiveData(ip, port, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugWebServer) {
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void WebClient::binaryFrameReceived(const QByteArray &data, bool isLastFrame)
{
    QString buffer;
    if (AppConfig::HexReceiveWebClient) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else {
        buffer = QString(data);
    }

    textFrameReceived(buffer, isLastFrame);
}

void WebClient::textMessageReceived(const QString &data)
{
    textFrameReceived(data, true);
}

void WebClient::binaryMessageReceived(const QByteArray &data)
{
    binaryFrameReceived(data, true);
}

void WebClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendWebServer) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    if (AppConfig::AsciiWebServer) {
        socket->sendTextMessage(data);
    } else {
        socket->sendBinaryMessage(buffer);
    }

    emit sendData(ip, port, data);
}

void WebClient::abort()
{
    socket->abort();
}
