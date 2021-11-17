#ifndef APPKEY_H
#define APPKEY_H

#include <QObject>
#include <QDateTime>

class QTimer;

class AppKey : public QObject
{
    Q_OBJECT
public:
    static AppKey *Instance();
    explicit AppKey(QObject *parent = 0);

private:
    static AppKey *self;

    QString keyData;            //注册码密文
    bool keyUseDate;            //是否启用运行日期时间限制
    QString keyDate;            //到期时间字符串
    bool keyUseRun;             //是否启用可运行时间限制
    int keyRun;                 //可运行时间
    bool keyUseCount;           //是否启用设备数量限制
    int keyCount;               //设备限制数量

    QTimer *timer;              //定时器判断是否运行超时
    QDateTime startTime;        //程序启动时间

private slots:
    void checkTime();
    QString getXorEncryptDecrypt(const QString &data, char key);

public slots:
    void start();
    void stop();
    bool checkCount(int count);
};

#endif // APPKEY_H
