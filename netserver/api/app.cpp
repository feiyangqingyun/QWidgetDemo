#include "app.h"
#include "quiwidget.h"

QString App::ConfigFile = "config.ini";

int App::ListenPort1 = 6907;
int App::CmdStart1 = 76;
int App::CmdLen1 = 12;
bool App::HexData1 = false;
int App::ListenPort2 = 6908;
int App::CmdStart2 = 76;
int App::CmdLen2 = 12;
bool App::HexData2 = false;

void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    App::ListenPort1 = set.value("ListenPort1").toInt();
    App::CmdStart1 = set.value("CmdStart1").toInt();
    App::CmdLen1 = set.value("CmdLen1").toInt();
    App::HexData1 = set.value("HexData1").toBool();
    App::ListenPort2 = set.value("ListenPort2").toInt();
    App::CmdStart2 = set.value("CmdStart2").toInt();
    App::CmdLen2 = set.value("CmdLen2").toInt();
    App::HexData2 = set.value("HexData2").toBool();
    set.endGroup();
}

void App::writeConfig()
{
    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    set.setValue("ListenPort1", App::ListenPort1);
    set.setValue("CmdStart1", App::CmdStart1);
    set.setValue("CmdLen1", App::CmdLen1);
    set.setValue("HexData1", App::HexData1);
    set.setValue("ListenPort2", App::ListenPort2);
    set.setValue("CmdStart2", App::CmdStart2);
    set.setValue("CmdLen2", App::CmdLen2);
    set.setValue("HexData2", App::HexData2);
    set.endGroup();
}

void App::newConfig()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
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
