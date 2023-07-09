#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";
QString AppConfig::SendFileName = "send.txt";
QString AppConfig::DeviceFileName = "device.txt";

QString AppConfig::PortName = "COM1";
int AppConfig::BaudRate = 9600;
int AppConfig::DataBit = 8;
QString AppConfig::Parity = QString::fromUtf8("无");
double AppConfig::StopBit = 1;

bool AppConfig::HexSend = false;
bool AppConfig::HexReceive = false;
bool AppConfig::Debug = false;
bool AppConfig::AutoClear = false;

bool AppConfig::AutoSend = false;
int AppConfig::SendInterval = 1000;
bool AppConfig::AutoSave = false;
int AppConfig::SaveInterval = 5000;

QString AppConfig::Mode = "Tcp_Client";
QString AppConfig::ServerIP = "127.0.0.1";
int AppConfig::ServerPort = 6000;
int AppConfig::ListenPort = 6000;
int AppConfig::SleepTime = 100;
bool AppConfig::AutoConnect = false;

void AppConfig::readConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    AppConfig::PortName = set.value("PortName", AppConfig::PortName).toString();
    AppConfig::BaudRate = set.value("BaudRate", AppConfig::BaudRate).toInt();
    AppConfig::DataBit = set.value("DataBit", AppConfig::DataBit).toInt();
    AppConfig::Parity = set.value("Parity", AppConfig::Parity).toString();
    AppConfig::StopBit = set.value("StopBit", AppConfig::StopBit).toInt();

    AppConfig::HexSend = set.value("HexSend", AppConfig::HexSend).toBool();
    AppConfig::HexReceive = set.value("HexReceive", AppConfig::HexReceive).toBool();
    AppConfig::Debug = set.value("Debug", AppConfig::Debug).toBool();
    AppConfig::AutoClear = set.value("AutoClear", AppConfig::AutoClear).toBool();

    AppConfig::AutoSend = set.value("AutoSend", AppConfig::AutoSend).toBool();
    AppConfig::SendInterval = set.value("SendInterval", AppConfig::SendInterval).toInt();
    AppConfig::AutoSave = set.value("AutoSave", AppConfig::AutoSave).toBool();
    AppConfig::SaveInterval = set.value("SaveInterval", AppConfig::SaveInterval).toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    AppConfig::Mode = set.value("Mode", AppConfig::Mode).toString();
    AppConfig::ServerIP = set.value("ServerIP", AppConfig::ServerIP).toString();
    AppConfig::ServerPort = set.value("ServerPort", AppConfig::ServerPort).toInt();
    AppConfig::ListenPort = set.value("ListenPort", AppConfig::ListenPort).toInt();
    AppConfig::SleepTime = set.value("SleepTime", AppConfig::SleepTime).toInt();
    AppConfig::AutoConnect = set.value("AutoConnect", AppConfig::AutoConnect).toBool();
    set.endGroup();

    //配置文件不存在或者不全则重新生成
    if (!QUIHelper::checkIniFile(AppConfig::ConfigFile)) {
        writeConfig();
        return;
    }
}

void AppConfig::writeConfig()
{    
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    set.setValue("PortName", AppConfig::PortName);
    set.setValue("BaudRate", AppConfig::BaudRate);
    set.setValue("DataBit", AppConfig::DataBit);
    set.setValue("Parity", AppConfig::Parity);
    set.setValue("StopBit", AppConfig::StopBit);

    set.setValue("HexSend", AppConfig::HexSend);
    set.setValue("HexReceive", AppConfig::HexReceive);
    set.setValue("Debug", AppConfig::Debug);
    set.setValue("AutoClear", AppConfig::AutoClear);

    set.setValue("AutoSend", AppConfig::AutoSend);
    set.setValue("SendInterval", AppConfig::SendInterval);
    set.setValue("AutoSave", AppConfig::AutoSave);
    set.setValue("SaveInterval", AppConfig::SaveInterval);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("Mode", AppConfig::Mode);
    set.setValue("ServerIP", AppConfig::ServerIP);
    set.setValue("ServerPort", AppConfig::ServerPort);
    set.setValue("ListenPort", AppConfig::ListenPort);
    set.setValue("SleepTime", AppConfig::SleepTime);
    set.setValue("AutoConnect", AppConfig::AutoConnect);
    set.endGroup();
}
