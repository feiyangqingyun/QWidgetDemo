#pragma execution_character_set("utf-8")

#include "applive.h"
#include "qmutex.h"
#include "qudpsocket.h"
#include "qstringlist.h"
#include "qapplication.h"
#include "qdatetime.h"
#include "qdebug.h"

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

QScopedPointer<AppLive> AppLive::self;
AppLive *AppLive::Instance()
{
    if (self.isNull()) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppLive);
        }
    }

    return self.data();
}

AppLive::AppLive(QObject *parent) : QObject(parent)
{
    udpServer  = new QUdpSocket(this);

    QString name = qApp->applicationFilePath();
    QStringList list = name.split("/");
    appName = list.at(list.count() - 1).split(".").at(0);
}

void AppLive::readData()
{
    QByteArray tempData;

    do {
        tempData.resize(udpServer->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpServer->readDatagram(tempData.data(), tempData.size(), &sender, &senderPort);
        QString data = QLatin1String(tempData);

        if (data == "hello") {
            udpServer->writeDatagram(QString("%1OK").arg(appName).toLatin1(), sender, senderPort);
        }
    } while (udpServer->hasPendingDatagrams());
}

bool AppLive::start(int port)
{
    bool ok = udpServer->bind(port);
    if (ok) {
        connect(udpServer, SIGNAL(readyRead()), this, SLOT(readData()));
        qDebug() << TIMEMS << "Start AppLive Ok";
    }

    return ok;
}

void AppLive::stop()
{
    udpServer->abort();
    disconnect(udpServer, SIGNAL(readyRead()), this, SLOT(readData()));
}
