#pragma execution_character_set("utf-8")

#include "savelog.h"
#include "qmutex.h"
#include "qdir.h"
#include "qfile.h"
#include "qtcpsocket.h"
#include "qtcpserver.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qtextstream.h"
#include "qstringlist.h"

#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QDATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

//日志重定向
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
void Log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
void Log(QtMsgType type, const char *msg)
#endif
{
    //加锁,防止多线程中qdebug太频繁导致崩溃
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString content;

    //这里可以根据不同的类型加上不同的头部用于区分
    int msgType = SaveLog::Instance()->getMsgType();
    switch (type) {
        case QtDebugMsg:
            if ((msgType & MsgType_Debug) == MsgType_Debug) {
                content = QString("Debug %1").arg(msg);
            }
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5,5,0))
        case QtInfoMsg:
            if ((msgType & MsgType_Info) == MsgType_Info) {
                content = QString("Infox %1").arg(msg);
            }
            break;
#endif
        case QtWarningMsg:
            if ((msgType & MsgType_Warning) == MsgType_Warning) {
                content = QString("Warnx %1").arg(msg);
            }
            break;
        case QtCriticalMsg:
            if ((msgType & MsgType_Critical) == MsgType_Critical) {
                content = QString("Error %1").arg(msg);
            }
            break;
        case QtFatalMsg:
            if ((msgType & MsgType_Fatal) == MsgType_Fatal) {
                content = QString("Fatal %1").arg(msg);
            }
            break;
    }

    //没有内容则返回
    if (content.isEmpty()) {
        return;
    }

    //加上打印代码所在代码文件、行号、函数名
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    if (SaveLog::Instance()->getUseContext()) {
        int line = context.line;
        QString file = context.file;
        QString function = context.function;
        if (line > 0) {
            content = QString("行号: %1  文件: %2  函数: %3\n%4").arg(line).arg(file).arg(function).arg(content);
        }
    }
#endif

    //还可以将数据转成html内容分颜色区分
    //将内容传给函数进行处理
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

    isRun = false;
    maxRow = currentRow = 0;
    maxSize = 128;
    toNet = false;
    useContext = true;

    //全局的文件对象,在需要的时候打开而不是每次添加日志都打开
    file = new QFile(this);
    //默认取应用程序根目录
    path = qApp->applicationDirPath();
    //默认取应用程序可执行文件名称
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
    fileName = "";

    //默认所有类型都输出
    msgType = MsgType(MsgType_Debug | MsgType_Info | MsgType_Warning | MsgType_Critical | MsgType_Fatal);
}

SaveLog::~SaveLog()
{
    this->stop();
}

void SaveLog::openFile(const QString &fileName)
{
    //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
    if (this->fileName != fileName) {
        this->fileName = fileName;
        //先关闭之前的
        if (file->isOpen()) {
            file->close();
        }
        //重新设置新的日志文件
        file->setFileName(fileName);
        //以 Append 追加的形式打开
        file->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    }
}

bool SaveLog::getUseContext()
{
    return this->useContext;
}

MsgType SaveLog::getMsgType()
{
    return this->msgType;
}

//安装日志钩子,输出调试信息到文件,便于调试
void SaveLog::start()
{
    if (isRun) {
        return;
    }

    isRun = true;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qInstallMessageHandler(Log);
#else
    qInstallMsgHandler(Log);
#endif
}

//卸载日志钩子
void SaveLog::stop()
{
    if (!isRun) {
        return;
    }

    isRun = false;
    this->clear();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qInstallMessageHandler(0);
#else
    qInstallMsgHandler(0);
#endif
}

void SaveLog::clear()
{
    currentRow = 0;
    fileName.clear();
    if (file->isOpen()) {
        file->close();
    }
}

void SaveLog::save(const QString &content)
{
    //如果重定向输出到网络则通过网络发出去,否则输出到日志文件
    if (toNet) {
        emit send(content);
    } else {
        //目录不存在则先新建目录
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkdir(path);
        }

        //日志存储规则有多种策略 优先级 行数>大小>日期
        //1: 设置了最大行数限制则按照行数限制来
        //2: 设置了大小则按照大小来控制日志文件
        //3: 都没有设置都存储到日期命名的文件,只有当日期变化了才会切换到新的日志文件
        bool needOpen = false;
        if (maxRow > 0) {
            currentRow++;
            if (fileName.isEmpty()) {
                needOpen = true;
            } else if (currentRow >= maxRow) {
                needOpen = true;
            }
        } else if (maxSize > 0) {
            //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
            //首次需要重新打开文件以及超过大小需要重新打开文件
            if (fileName.isEmpty()) {
                needOpen = true;
            } else if (file->size() > (maxSize * 1024)) {
                needOpen = true;
            }
        } else {
            //日期改变了才会触发
            QString fileName = QString("%1/%2_log_%3.txt").arg(path).arg(name).arg(QDATE);
            openFile(fileName);
        }

        if ((maxRow > 0 || maxSize > 0) && needOpen) {
            currentRow = 0;
            QString fileName = QString("%1/%2_log_%3.txt").arg(path).arg(name).arg(QDATETIMS);
            openFile(fileName);
        }

        //用文本流的输出速度更快
        QTextStream stream(file);
        stream << content << "\n";
    }
}

void SaveLog::setMaxRow(int maxRow)
{
    //这里可以限定最大最小值
    if (maxRow >= 0) {
        this->maxRow = maxRow;
        this->clear();
    }
}

void SaveLog::setMaxSize(int maxSize)
{
    //这里可以限定最大最小值
    if (maxSize >= 0) {
        this->maxSize = maxSize;
        this->clear();
    }
}

void SaveLog::setListenPort(int listenPort)
{
    SendLog::Instance()->setListenPort(listenPort);
}

void SaveLog::setToNet(bool toNet)
{
    this->toNet = toNet;
    if (toNet) {
        SendLog::Instance()->start();
    } else {
        SendLog::Instance()->stop();
    }
}

void SaveLog::setUseContext(bool useContext)
{
    this->useContext = useContext;
}

void SaveLog::setPath(const QString &path)
{
    this->path = path;
}

void SaveLog::setName(const QString &name)
{
    this->name = name;
}

void SaveLog::setMsgType(const MsgType &msgType)
{
    this->msgType = msgType;
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

SendLog::SendLog(QObject *parent) : QObject(parent)
{
    listenPort = 6000;
    socket = NULL;

    //实例化网络通信服务器对象
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
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
    //限定就一个连接
    while (server->hasPendingConnections()) {
        socket = server->nextPendingConnection();
    }
}

void SendLog::setListenPort(int listenPort)
{
    this->listenPort = listenPort;
}

void SendLog::start()
{
    //启动端口监听
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    server->listen(QHostAddress::AnyIPv4, listenPort);
#else
    server->listen(QHostAddress::Any, listenPort);
#endif
}

void SendLog::stop()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
        socket = NULL;
    }

    server->close();
}

void SendLog::send(const QString &content)
{
    if (socket != NULL && socket->isOpen()) {
        socket->write(content.toUtf8());
        //socket->flush();
    }
}
