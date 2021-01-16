#ifndef SAVERUNTIME_H
#define SAVERUNTIME_H

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

private:
    static QScopedPointer<SaveRunTime> self;
    QString path;       //日志文件路径
    QString name;       //日志文件名称

    int lastID;
    int saveInterval;
    QDateTime startTime;
    QString logFile;
    QTimer *timerSave;

private:
    void getDiffValue(const QDateTime &startTime, const QDateTime &endTime, int &day, int &hour, int &minute);

signals:

public slots:
    void start();       //启动服务
    void stop();        //停止服务
    void initLog();     //初始化日志文件
    void appendLog();   //追加一条记录到日志文件
    void saveLog();     //保存运行时间到日志文件

    void setPath(const QString &path);
    void setName(const QString &name);
    void setSaveInterval(int saveInterval);
};

#endif // SAVERUNTIME_H
