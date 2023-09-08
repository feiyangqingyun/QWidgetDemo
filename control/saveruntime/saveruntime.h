#ifndef SAVERUNTIME_H
#define SAVERUNTIME_H

/**
 * 运行时间记录 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1. 可以启动和停止服务，在需要的时候启动。
 * 2. 可以指定日志文件存放目录。
 * 3. 可以指定时间日志输出间隔。
 * 4. 可以单独追加一条记录到日志文件。
 * 5. 日志为文本格式，清晰明了。
 */

#include <QObject>
#include <QDateTime>

class QTimer;

#ifdef quc
class Q_DECL_EXPORT SaveRunTime : public QObject
#else
class SaveRunTime : public QObject
#endif

{
    Q_OBJECT
public:
    static SaveRunTime *Instance();
    explicit SaveRunTime(QObject *parent = 0);
    ~SaveRunTime();

private:
    static QScopedPointer<SaveRunTime> self;

    //日志文件路径
    QString path;
    //日志文件名称
    QString name;

    //最后的编号
    int lastID;
    //保存间隔
    int saveInterval;
    //开始时间
    QDateTime startTime;
    //日志文件
    QString logFile;
    //保存文件定时器
    QTimer *timerSave;

private:
    //比较两个时间差值
    void getDiffValue(const QDateTime &startTime, const QDateTime &endTime, int &day, int &hour, int &minute);

public Q_SLOTS:
    //启动服务
    void start();
    //停止服务
    void stop();

    //新建目录
    void newPath();
    //初始化日志文件
    void initLog();
    //追加一条记录到日志文件
    void appendLog();
    //保存运行时间到日志文件
    void saveLog();

    //设置文件保存目录
    void setPath(const QString &path);
    //设置文件名称
    void setName(const QString &name);
    //设置保存间隔
    void setSaveInterval(int saveInterval);
};

#endif // SAVERUNTIME_H
