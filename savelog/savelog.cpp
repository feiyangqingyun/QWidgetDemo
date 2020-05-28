#include "savelog.h"
#include "qmutex.h"
#include "qfile.h"
#include "qtcpsocket.h"
#include "qtcpserver.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qstringlist.h"

#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))

//日志重定向
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
void Log(QtMsgType type, const char *msg)
#else
void Log(QtMsgType type, const QMessageLogContext &, const QString &msg)
#endif
{
    //加锁,防止多线程中qdebug太频繁导致崩溃
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString content;

    //这里可以根据不同的类型加上不同的头部用于区分
    switch (type) {
    case QtDebugMsg:
        content = QString("%1").arg(msg);
        break;

    case QtWarningMsg:
        content = QString("%1").arg(msg);
        break;

    case QtCriticalMsg:
        content = QString("%1").arg(msg);
        break;

    case QtFatalMsg:
        content = QString("%1").arg(msg);
        break;
    }

    SaveLog::Instance()->save(content);
}

QScopedPointer<SaveLog> SaveLog::self;
SaveLog *SaveLog::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SaveLog);
        }
    }

    return self.data();
}

SaveLog::SaveLog(QObject *parent) : QObject(parent)
{
    //必须用信号槽形式,不然提示 QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread
    //估计日志钩子可能单独开了线程
    connect(this, SIGNAL(send(QString)), SendLog::Instance(), SLOT(send(QString)));

    file = new QFile(this);
    toNet = false;
    //默认取应用程序根目录
    path = qApp->applicationDirPath();
    //默认取应用程序可执行文件名称
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
    fileName = "";
}

SaveLog::~SaveLog()
{
    file->close();    
}

//安装日志钩子,输出调试信息到文件,便于调试
void SaveLog::start()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    qInstallMsgHandler(Log);
#else
    qInstallMessageHandler(Log);
#endif
}

//卸载日志钩子
void SaveLog::stop()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    qInstallMsgHandler(0);
#else
    qInstallMessageHandler(0);
#endif
}

void SaveLog::save(const QString &content)
{
    //如果重定向输出到网络则通过网络发出去,否则输出到日志文件
    if (toNet) {
        emit send(content);
    } else {
        //方法改进:之前每次输出日志都打开文件,改成只有当日期改变时才新建和打开文件
        QString fileName = QString("%1/%2_log_%3.txt").arg(path).arg(name).arg(QDATE);
        if (this->fileName != fileName) {
            this->fileName = fileName;
            if (file->isOpen()) {
                file->close();
            }

            file->setFileName(fileName);
            file->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }

        QTextStream logStream(file);
        logStream << content << "\n";
    }
}

void SaveLog::setToNet(bool toNet)
{
    this->toNet = toNet;
}

void SaveLog::setPath(const QString &path)
{
    this->path = path;
}

void SaveLog::setName(const QString &name)
{
    this->name = name;
}


//网络发送日志数据类
QScopedPointer<SendLog> SendLog::self;
SendLog *SendLog::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SendLog);
        }
    }

    return self.data();
}

SendLog::SendLog(QObject *parent)
{
    socket = NULL;
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    int listenPort = 6000;
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    server->listen(QHostAddress::AnyIPv4, listenPort);
#else
    server->listen(QHostAddress::Any, listenPort);
#endif
}

SendLog::~SendLog()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
    }

    server->close();
}

void SendLog::newConnection()
{
    while (server->hasPendingConnections()) {
        socket = server->nextPendingConnection();
    }
}

void SendLog::send(const QString &content)
{
    if (socket != NULL && socket->isOpen()) {
        socket->write(content.toUtf8());
        socket->flush();
    }
}
