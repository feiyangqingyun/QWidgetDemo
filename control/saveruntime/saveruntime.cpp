#pragma execution_character_set("utf-8")

#include "saveruntime.h"
#include "qmutex.h"
#include "qdir.h"
#include "qfile.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qtextstream.h"
#include "qstringlist.h"

QScopedPointer<SaveRunTime> SaveRunTime::self;
SaveRunTime *SaveRunTime::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SaveRunTime);
        }
    }

    return self.data();
}

SaveRunTime::SaveRunTime(QObject *parent) : QObject(parent)
{
    path = qApp->applicationDirPath();
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);

    saveInterval = 1 * 60 * 1000;
    startTime = QDateTime::currentDateTime();

    //存储运行时间定时器
    timerSave = new QTimer(this);
    timerSave->setInterval(saveInterval);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveLog()));
}

SaveRunTime::~SaveRunTime()
{
    this->stop();
}

void SaveRunTime::getDiffValue(const QDateTime &startTime, const QDateTime &endTime, int &day, int &hour, int &minute)
{
    qint64 sec = startTime.secsTo(endTime);
    day = hour = minute = 0;
    int seconds = 0;

    while (sec > 0) {
        seconds++;
        if (seconds == 60) {
            minute++;
            seconds = 0;
        }

        if (minute == 60) {
            hour++;
            minute = 0;
        }

        if (hour == 24) {
            day++;
            hour = 0;
        }

        sec--;
    }
}

void SaveRunTime::start()
{
    if (timerSave->isActive()) {
        return;
    }

    //开始时间变量必须在这,在部分嵌入式系统上开机后的时间不准确比如是1970,而后会变成1999或者其他时间
    //会在getDiffValue函数执行很久很久
    startTime = QDateTime::currentDateTime();
    timerSave->start();

    initLog();
    appendLog();
    saveLog();
}

void SaveRunTime::stop()
{
    if (!timerSave->isActive()) {
        return;
    }

    timerSave->stop();
}

void SaveRunTime::newPath()
{
    //检查目录是否存在,不存在则先新建
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkdir(path);
    }
}

void SaveRunTime::initLog()
{
    //判断当前年份的记事本文件是否存在,不存在则新建并且写入标题
    //存在则自动读取最后一行的id号  记事本文件格式内容
    //幢号    开始时间                结束时间                已运行时间
    //1      2016-01-01 12:33:33    2016-02-05 12:12:12     day: 0  hour: 0  minute: 0

    newPath();
    logFile = QString("%1/%2_runtime_%3.txt").arg(path).arg(name).arg(QDate::currentDate().year());
    QFile file(logFile);

    if (file.size() == 0) {
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QString strID = QString("%1\t").arg("编号");
            QString strStartTime = QString("%1\t\t").arg("开始时间");
            QString strEndTime = QString("%1\t\t").arg("结束时间");
            QString strRunTime = QString("%1").arg("已运行时间");
            QString line = strID + strStartTime + strEndTime + strRunTime;

            QTextStream stream(&file);
            stream << line << "\n";
            file.close();
            lastID = 0;
        }
    } else {
        if (file.open(QFile::ReadOnly)) {
            QString lastLine;
            while (!file.atEnd()) {
                lastLine = file.readLine();
            }

            file.close();
            QStringList list = lastLine.split("\t");
            lastID = list.at(0).toInt();
        }
    }

    lastID++;
}

void SaveRunTime::appendLog()
{
    newPath();
    logFile = QString("%1/%2_runtime_%3.txt").arg(path).arg(name).arg(QDate::currentDate().year());
    QFile file(logFile);

    //写入当前首次运行时间
    if (file.open(QFile::WriteOnly | QFile::Append | QFile::Text)) {
        QString strID = QString("%1\t").arg(lastID);
        QString strStartTime = QString("%1\t").arg(startTime.toString("yyyy-MM-dd HH:mm:ss"));
        QString strEndTime = QString("%1\t").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        int day, hour, minute;
        getDiffValue(startTime, QDateTime::currentDateTime(), day, hour, minute);
        QString strRunTime = QString("%1 天 %2 时 %3 分").arg(day).arg(hour).arg(minute);
        QString line = strID + strStartTime + strEndTime + strRunTime;

        QTextStream stream(&file);
        stream << line << "\n";
        file.close();
    }
}

void SaveRunTime::saveLog()
{
    //每次保存都是将之前的所有文本读取出来,然后替换最后一行即可
    newPath();
    logFile = QString("%1/%2_runtime_%3.txt").arg(path).arg(name).arg(QDate::currentDate().year());
    QFile file(logFile);

    //如果日志文件不存在,则初始化一个日志文件
    if (file.size() == 0) {
        initLog();
        appendLog();
        return;
    }

    if (file.open(QFile::ReadWrite)) {
        //一行行读取到链表
        QStringList content;
        while (!file.atEnd()) {
            content.append(file.readLine());
        }

        //重新清空文件
        file.resize(0);
        //如果行数小于2则返回
        if (content.count() < 2) {
            file.close();
            return;
        }

        QString lastLine = content.last();
        QStringList list = lastLine.split("\t");

        //计算已运行时间
        int day, hour, minute;
        getDiffValue(startTime, QDateTime::currentDateTime(), day, hour, minute);
        QString strRunTime = QString("%1 天 %2 时 %3 分").arg(day).arg(hour).arg(minute);

        //重新拼接最后一行
        list[2] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        list[3] = strRunTime;
        lastLine = list.join("\t");

        //重新替换最后一行并写入新的数据
        content[content.count() - 1] = lastLine;

        QTextStream stream(&file);
        stream << content.join("") << "\n";
        file.close();
    }
}

void SaveRunTime::setPath(const QString &path)
{
    this->path = path;
}

void SaveRunTime::setName(const QString &name)
{
    this->name = name;
}

void SaveRunTime::setSaveInterval(int saveInterval)
{
    if (this->saveInterval != saveInterval) {
        this->saveInterval = saveInterval;
        timerSave->setInterval(saveInterval);
    }
}
