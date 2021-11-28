#pragma execution_character_set("utf-8")

#include "appkey.h"
#include "qmutex.h"
#include "qfile.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qmessagebox.h"

AppKey *AppKey::self = NULL;
AppKey *AppKey::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new AppKey;
        }
    }

    return self;
}

AppKey::AppKey(QObject *parent) : QObject(parent)
{
    keyData = "";
    keyUseDate = false;
    keyDate = "2017-01-01";
    keyUseRun = false;
    keyRun = 1;
    keyUseCount = false;
    keyCount = 10;

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkTime()));
    startTime = QDateTime::currentDateTime();
}

void AppKey::start()
{
    //判断密钥文件是否存在,不存在则从资源文件复制出来,同时需要设置文件写权限
    QString keyName = qApp->applicationDirPath() + "/key.db";
    QFile keyFile(keyName);
    if (!keyFile.exists() || keyFile.size() == 0) {
        QMessageBox::critical(0, "错误", "密钥文件丢失,请联系供应商!");
        exit(0);
    }

    //读取密钥文件
    keyFile.open(QFile::ReadOnly);
    keyData = keyFile.readLine();
    keyFile.close();

    //将从注册码文件中的密文解密,与当前时间比较是否到期
    keyData = getXorEncryptDecrypt(keyData, 110);
    QStringList data = keyData.split("|");

    if (data.count() != 6) {
        QMessageBox::critical(0, "错误", "注册码文件已损坏,程序将自动关闭!");
        exit(0);
    }

    keyUseDate = (data.at(0) == "1");
    keyDate = data.at(1);
    keyUseRun = (data.at(2) == "1");
    keyRun = data.at(3).toInt();
    keyUseCount = (data.at(4) == "1");
    keyCount = data.at(5).toInt();

    //如果启用了时间限制
    if (keyUseDate) {
        QString nowDate = QDate::currentDate().toString("yyyy-MM-dd");
        if (nowDate > keyDate) {
            QMessageBox::critical(0, "错误", "软件已到期,请联系供应商更新注册码!");
            exit(0);
        }
    }

    //如果启用了运行时间显示
    if (keyUseRun) {
        timer->start();
    }
}

void AppKey::stop()
{
    timer->stop();
}

void AppKey::checkTime()
{
    //找出当前时间与首次启动时间比较
    QDateTime now = QDateTime::currentDateTime();
    if (startTime.secsTo(now) >= (keyRun * 60)) {
        QMessageBox::critical(0, "错误", "试运行时间已到,请联系供应商更新注册码!");
        exit(0);
    }
}

QString AppKey::getXorEncryptDecrypt(const QString &data, char key)
{
    //采用异或加密,也可以自行更改算法
    QByteArray buffer = data.toLatin1();
    int size = buffer.size();
    for (int i = 0; i < size; i++) {
        buffer[i] = buffer.at(i) ^ key;
    }

    return QLatin1String(buffer);
}

bool AppKey::checkCount(int count)
{
    if (keyUseCount) {
        if (count >= keyCount) {
            QMessageBox::critical(0, "错误", "设备数量超过限制,请联系供应商更新注册码!");
            return false;
        }
    }

    return true;
}
