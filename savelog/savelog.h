#ifndef SAVELOG_H
#define SAVELOG_H

/**
 * 日志重定向输出 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1. 支持动态启动和停止。
 * 2. 支持日志存储的目录。
 * 3. 支持网络发出打印日志。
 * 4. 支持Qt4+Qt5+Qt6，开箱即用。
 * 5. 支持多线程。
 * 6. 使用做到最简单，start即可。
 */

#include <QObject>

class QFile;
class QTcpSocket;
class QTcpServer;

#ifdef quc
class Q_DECL_EXPORT SaveLog : public QObject
#else
class SaveLog : public QObject
#endif

{
    Q_OBJECT
public:
    static SaveLog *Instance();
    explicit SaveLog(QObject *parent = 0);
    ~SaveLog();

private:
    static QScopedPointer<SaveLog> self;

    //文件对象
    QFile *file;
    //是否重定向到网络
    bool toNet;
    //日志文件路径
    QString path;
    //日志文件名称
    QString name;
    //日志文件完整名称
    QString fileName;

Q_SIGNALS:
    //发送内容信号
    void send(const QString &content);

public Q_SLOTS:
    //启动日志服务
    void start();
    //暂停日志服务
    void stop();
    //保存日志
    void save(const QString &content);

    //设置是否重定向到网络
    void setToNet(bool toNet);
    //设置日志文件存放路径
    void setPath(const QString &path);
    //设置日志文件名称
    void setName(const QString &name);
};

class SendLog : public QObject
{
    Q_OBJECT
public:
    static SendLog *Instance();
    explicit SendLog(QObject *parent = 0);
    ~SendLog();

private:
    static QScopedPointer<SendLog> self;

    //网络通信对象
    QTcpSocket *socket;
    //网络监听服务器
    QTcpServer *server;

private slots:
    //新连接到来
    void newConnection();

public Q_SLOTS:
    //发送日志
    void send(const QString &content);
};

#endif // SAVELOG_H
