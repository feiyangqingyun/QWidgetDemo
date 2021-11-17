#ifndef APPLIVE_H
#define APPLIVE_H

#include <QObject>

class QUdpSocket;

class AppLive : public QObject
{
	Q_OBJECT
public:
    static AppLive *Instance();
    explicit AppLive(QObject *parent = 0);	

private:
    static QScopedPointer<AppLive> self;
	QUdpSocket *udpServer;
    QString appName;

private slots:
    void readData();

public slots:
    bool start(int port);
    void stop();
};

#endif // APPLIVE_H
