#ifndef SAVELOG_H
#define SAVELOG_H

/**
 * 日志重定向输出 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1. 支持动态启动和停止。
 * 2. 支持日志存储的目录。
 * 3. 支持网络发出打印日志。
 * 4. 支持输出日志上下文信息比如所在代码文件、行号、函数名等。
 * 5. 支持设置日志文件大小限制，超过则自动分文件，默认128kb。
 * 6. 支持按照日志行数自动分文件，和日志大小条件互斥。
 * 7. 可选按照日期时间区分文件名存储日志。
 * 8. 日志文件命名规则优先级：行数》大小》日期。
 * 9. 自动加锁支持多线程。
 * 10. 可以分别控制哪些类型的日志需要重定向输出。
 * 11. 支持Qt4+Qt5+Qt6，开箱即用。
 * 12. 使用方式最简单，调用函数start()启动服务，stop()停止服务。
 */

#include <QObject>

class QFile;
class QTcpSocket;
class QTcpServer;

//消息类型
enum MsgType {
    MsgType_Debug = 0x0001,
    MsgType_Info = 0x0002,
    MsgType_Warning = 0x0004,
    MsgType_Critical = 0x0008,
    MsgType_Fatal = 0x0010,
};

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

    //是否在运行
    bool isRun;
    //文件最大行数 0表示不启用
    int maxRow, currentRow;
    //文件最大大小 0表示不启用 单位kb
    int maxSize;
    //是否重定向到网络
    bool toNet;
    //是否输出日志上下文
    bool useContext;

    //文件对象
    QFile *file;
    //日志文件路径
    QString path;
    //日志文件名称
    QString name;
    //日志文件完整名称
    QString fileName;
    //消息类型
    MsgType msgType;

private:
    void openFile(const QString &fileName);

public:
    bool getUseContext();
    MsgType getMsgType();

Q_SIGNALS:
    //发送内容信号
    void send(const QString &content);

public Q_SLOTS:
    //启动日志服务
    void start();
    //暂停日志服务
    void stop();

    //清空状态
    void clear();
    //保存日志
    void save(const QString &content);

    //设置日志文件最大行数
    void setMaxRow(int maxRow);
    //设置日志文件最大大小 单位kb
    void setMaxSize(int maxSize);

    //设置监听端口
    void setListenPort(int listenPort);
    //设置是否重定向到网络
    void setToNet(bool toNet);
    //设置是否输出日志上下文
    void setUseContext(bool useContext);

    //设置日志文件存放路径
    void setPath(const QString &path);
    //设置日志文件名称
    void setName(const QString &name);
    //设置消息类型
    void setMsgType(const MsgType &msgType);
};

#ifdef quc
class Q_DECL_EXPORT SendLog : public QObject
#else
class SendLog : public QObject
#endif

{
    Q_OBJECT
public:
    static SendLog *Instance();
    explicit SendLog(QObject *parent = 0);
    ~SendLog();

private:
    static QScopedPointer<SendLog> self;

    //监听端口
    int listenPort;
    //网络通信对象
    QTcpSocket *socket;
    //网络监听服务器
    QTcpServer *server;

private slots:
    //新连接到来
    void newConnection();

public Q_SLOTS:
    //设置监听端口
    void setListenPort(int listenPort);

    //启动和停止服务
    void start();
    void stop();

    //发送日志
    void send(const QString &content);
};

#endif // SAVELOG_H
