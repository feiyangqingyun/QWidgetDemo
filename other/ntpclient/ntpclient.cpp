#include "ntpclient.h"
#include "qmutex.h"
#include "qudpsocket.h"
#include "qdebug.h"

QScopedPointer<NtpClient> NtpClient::self;
NtpClient *NtpClient::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new NtpClient);
        }
    }

    return self.data();
}

NtpClient::NtpClient(QObject *parent) : QObject(parent)
{
    ntpIP = "ntp1.aliyun.com";

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(connected()), this, SLOT(sendData()));
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void NtpClient::sendData()
{
    qint8 LI = 0;
    qint8 VN = 3;
    qint8 MODE = 3;
    qint8 STRATUM = 0;
    qint8 POLL = 4;
    qint8 PREC = -6;
    QDateTime epoch(QDate(1900, 1, 1), QTime(0, 0, 0));
    qint32 second = quint32(epoch.secsTo(QDateTime::currentDateTime()));

    qint32 temp = 0;
    QByteArray timeRequest(48, 0);
    timeRequest[0] = (LI << 6) | (VN << 3) | (MODE);
    timeRequest[1] = STRATUM;
    timeRequest[2] = POLL;
    timeRequest[3] = PREC & 0xff;
    timeRequest[5] = 1;
    timeRequest[9] = 1;
    timeRequest[40] = (temp = (second & 0xff000000) >> 24);
    temp = 0;
    timeRequest[41] = (temp = (second & 0x00ff0000) >> 16);
    temp = 0;
    timeRequest[42] = (temp = (second & 0x0000ff00) >> 8);
    temp = 0;
    timeRequest[43] = ((second & 0x000000ff));

    udpSocket->write(timeRequest);
}

void NtpClient::setTime_t(uint secsSince1Jan1970UTC)
{

}

void NtpClient::readData()
{
    QByteArray newTime;
    QDateTime epoch(QDate(1900, 1, 1), QTime(0, 0, 0));
    QDateTime unixStart(QDate(1970, 1, 1), QTime(0, 0, 0));

    while (udpSocket->hasPendingDatagrams()) {
        newTime.resize(udpSocket->pendingDatagramSize());
        udpSocket->read(newTime.data(), newTime.size());
    };

    QByteArray transmitTimeStamp ;
    transmitTimeStamp = newTime.right(8);
    quint32 seconds = transmitTimeStamp.at(0);
    quint8 temp = 0;

    for (int i = 1; i <= 3; ++i) {
        seconds = (seconds << 8);
        temp = transmitTimeStamp.at(i);
        seconds = seconds + temp;
    }

    QDateTime dateTime;
    uint secs = seconds - epoch.secsTo(unixStart);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    dateTime.setSecsSinceEpoch(secs);
#else
    dateTime.setTime_t(secs);
#endif

#ifdef __arm__
#ifdef arma9
    dateTime = dateTime.addSecs(8 * 60 * 60);
#endif
#endif
    udpSocket->disconnectFromHost();

    //有些时候返回的数据可能有误或者解析不正确,导致填充的时间不正确
    if (dateTime.isValid()) {
        emit receiveTime(dateTime);
    }
}

void NtpClient::setNtpIP(const QString &ntpIP)
{
    if (this->ntpIP != ntpIP) {
        this->ntpIP = ntpIP;
    }
}

void NtpClient::getDateTime()
{
    udpSocket->abort();
    udpSocket->connectToHost(ntpIP, 123);
}
