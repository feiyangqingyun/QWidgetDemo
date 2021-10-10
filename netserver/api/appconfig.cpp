#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";

int AppConfig::ListenPort1 = 6907;
int AppConfig::CmdStart1 = 76;
int AppConfig::CmdLen1 = 12;
bool AppConfig::HexData1 = false;

int AppConfig::ListenPort2 = 6908;
int AppConfig::CmdStart2 = 76;
int AppConfig::CmdLen2 = 12;
bool AppConfig::HexData2 = false;

void AppConfig::readConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig1");
    AppConfig::ListenPort1 = set.value("ListenPort1", AppConfig::ListenPort1).toInt();
    AppConfig::CmdStart1 = set.value("CmdStart1", AppConfig::CmdStart1).toInt();
    AppConfig::CmdLen1 = set.value("CmdLen1", AppConfig::CmdLen1).toInt();
    AppConfig::HexData1 = set.value("HexData1", AppConfig::HexData1).toBool();
    set.endGroup();

    set.beginGroup("AppConfig2");
    AppConfig::ListenPort2 = set.value("ListenPort2", AppConfig::ListenPort2).toInt();
    AppConfig::CmdStart2 = set.value("CmdStart2", AppConfig::CmdStart2).toInt();
    AppConfig::CmdLen2 = set.value("CmdLen2", AppConfig::CmdLen2).toInt();
    AppConfig::HexData2 = set.value("HexData2", AppConfig::HexData2).toBool();
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

    set.beginGroup("AppConfig1");
    set.setValue("ListenPort1", AppConfig::ListenPort1);
    set.setValue("CmdStart1", AppConfig::CmdStart1);
    set.setValue("CmdLen1", AppConfig::CmdLen1);
    set.setValue("HexData1", AppConfig::HexData1);
    set.endGroup();

    set.beginGroup("AppConfig2");
    set.setValue("ListenPort2", AppConfig::ListenPort2);
    set.setValue("CmdStart2", AppConfig::CmdStart2);
    set.setValue("CmdLen2", AppConfig::CmdLen2);
    set.setValue("HexData2", AppConfig::HexData2);
    set.endGroup();
}
