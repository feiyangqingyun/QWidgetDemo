#include "tcpclient.h"
#include "quihelper.h"
#include "quihelperdata.h"

TcpClient::TcpClient(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    this->socket = socket;
    ip = socket->peerAddress().toString();
    ip = ip.replace("::ffff:", "");
    port = socket->peerPort();

    connect(socket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slot_readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slot_error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error()));
#endif
}

QString TcpClient::getIP() const
{
    return this->ip;
}

int TcpClient::getPort() const
{
    return this->port;
}

void TcpClient::slot_disconnected()
{
    emit disconnected(ip, port);
    socket->deleteLater();
    this->deleteLater();
}

void TcpClient::slot_error()
{
    emit error(ip, port, socket->errorString());
}

void TcpClient::slot_readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (AppConfig::HexReceiveTcpServer) {
        buffer = QUIHelperData::byteArrayToHexStr(data);
    } else if (AppConfig::AsciiTcpServer) {
        buffer = QUIHelperData::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    emit receiveData(ip, port, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugTcpServer) {
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void TcpClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendTcpServer) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiTcpServer) {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->write(buffer);
    emit sendData(ip, port, data);
}

void TcpClient::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void TcpClient::abort()
{
    socket->abort();
}
