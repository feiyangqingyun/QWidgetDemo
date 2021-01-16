#ifndef SAVELOG_H
#define SAVELOG_H

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

signals:
    void send(const QString &content);

public slots:
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
    QTcpSocket *socket;
    QTcpServer *server;

private slots:
    void newConnection();

public slots:
    //发送日志
    void send(const QString &content);
};

#endif // SAVELOG_H
