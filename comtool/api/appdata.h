#ifndef APPDATA_H
#define APPDATA_H

#include "head.h"

class AppData
{
public:
    //全局变量
    static QStringList Intervals;
    static QStringList Datas;
    static QStringList Keys;
    static QStringList Values;

    //读取发送数据列表
    static QString SendFileName;
    static void readSendData();

    //读取转发数据列表
    static QString DeviceFileName;
    static void readDeviceData();

    //保存数据到文件
    static void saveData(const QString &data);

    //添加网卡IP地址到下拉框
    static void loadIP(QComboBox *cbox);
};

#endif // APPDATA_H
