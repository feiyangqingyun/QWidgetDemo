#include "app.h"
#include "quiwidget.h"

QString App::ConfigFile = "config.ini";
QString App::SendFileName = "send.txt";
QString App::DeviceFileName = "device.txt";

QString App::PortName = "COM1";
int App::BaudRate = 9600;
int App::DataBit = 8;
QString App::Parity = "无";
double App::StopBit = 1;

bool App::HexSend = false;
bool App::HexReceive = false;
bool App::Debug = false;
bool App::AutoClear = false;

bool App::AutoSend = false;
int App::SendInterval = 1000;
bool App::AutoSave = false;
int App::SaveInterval = 5000;

QString App::Mode = "Tcp_Client";
QString App::ServerIP = "127.0.0.1";
int App::ServerPort = 6000;
int App::ListenPort = 6000;
int App::SleepTime = 100;
bool App::AutoConnect = false;

void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    App::PortName = set.value("PortName").toString();
    App::BaudRate = set.value("BaudRate").toInt();
    App::DataBit = set.value("DataBit").toInt();
    App::Parity = set.value("Parity").toString();
    App::StopBit = set.value("StopBit").toInt();

    App::HexSend = set.value("HexSend").toBool();
    App::HexReceive = set.value("HexReceive").toBool();
    App::Debug = set.value("Debug").toBool();
    App::AutoClear = set.value("AutoClear").toBool();

    App::AutoSend = set.value("AutoSend").toBool();
    App::SendInterval = set.value("SendInterval").toInt();
    App::AutoSave = set.value("AutoSave").toBool();
    App::SaveInterval = set.value("SaveInterval").toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    App::Mode = set.value("Mode").toString();
    App::ServerIP = set.value("ServerIP").toString();
    App::ServerPort = set.value("ServerPort").toInt();
    App::ListenPort = set.value("ListenPort").toInt();
    App::SleepTime = set.value("SleepTime").toInt();
    App::AutoConnect = set.value("AutoConnect").toBool();
    set.endGroup();
}

void App::writeConfig()
{    
    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    set.setValue("PortName", App::PortName);
    set.setValue("BaudRate", App::BaudRate);
    set.setValue("DataBit", App::DataBit);
    set.setValue("Parity", App::Parity);
    set.setValue("StopBit", App::StopBit);

    set.setValue("HexSend", App::HexSend);
    set.setValue("HexReceive", App::HexReceive);
    set.setValue("Debug", App::Debug);
    set.setValue("AutoClear", App::AutoClear);

    set.setValue("AutoSend", App::AutoSend);
    set.setValue("SendInterval", App::SendInterval);
    set.setValue("AutoSave", App::AutoSave);
    set.setValue("SaveInterval", App::SaveInterval);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("Mode", App::Mode);
    set.setValue("ServerIP", App::ServerIP);
    set.setValue("ServerPort", App::ServerPort);
    set.setValue("ListenPort", App::ListenPort);
    set.setValue("SleepTime", App::SleepTime);
    set.setValue("AutoConnect", App::AutoConnect);
    set.endGroup();
}

void App::newConfig()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    App::Parity = App::Parity.toLatin1();
#endif
    writeConfig();
}

bool App::checkConfig()
{
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(App::ConfigFile);
    if (file.size() == 0) {
        newConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            newConfig();
            return false;
        }
    } else {
        newConfig();
        return false;
    }

    return true;
}

QStringList App::Intervals = QStringList();
QStringList App::Datas = QStringList();
QStringList App::Keys = QStringList();
QStringList App::Values = QStringList();

void App::readSendData()
{
    //读取发送数据列表
    App::Datas.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(App::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                App::Datas.append(line);
            }
        }

        file.close();
    }
}

void App::readDeviceData()
{
    //读取转发数据列表
    App::Keys.clear();
    App::Values.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(App::DeviceFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                App::Keys.append(key);
                App::Values.append(value);
            }
        }

        file.close();
    }
}
