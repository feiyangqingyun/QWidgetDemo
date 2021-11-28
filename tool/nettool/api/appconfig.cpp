#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";
int AppConfig::CurrentIndex = 0;

bool AppConfig::HexSendTcpClient = false;
bool AppConfig::HexReceiveTcpClient = false;
bool AppConfig::AsciiTcpClient = false;
bool AppConfig::DebugTcpClient = false;
bool AppConfig::AutoSendTcpClient = false;
int AppConfig::IntervalTcpClient = 1000;
QString AppConfig::TcpBindIP = "127.0.0.1";
int AppConfig::TcpBindPort = 6001;
QString AppConfig::TcpServerIP = "127.0.0.1";
int AppConfig::TcpServerPort = 6000;

bool AppConfig::HexSendTcpServer = false;
bool AppConfig::HexReceiveTcpServer = false;
bool AppConfig::AsciiTcpServer = false;
bool AppConfig::DebugTcpServer = false;
bool AppConfig::AutoSendTcpServer = false;
int AppConfig::IntervalTcpServer = 1000;
QString AppConfig::TcpListenIP = "127.0.0.1";
int AppConfig::TcpListenPort = 6000;
bool AppConfig::SelectAllTcpServer = true;

bool AppConfig::HexSendUdpClient = false;
bool AppConfig::HexReceiveUdpClient = false;
bool AppConfig::AsciiUdpClient = false;
bool AppConfig::DebugUdpClient = false;
bool AppConfig::AutoSendUdpClient = false;
int AppConfig::IntervalUdpClient = 1000;
QString AppConfig::UdpBindIP = "127.0.0.1";
int AppConfig::UdpBindPort = 6001;
QString AppConfig::UdpServerIP = "127.0.0.1";
int AppConfig::UdpServerPort = 6000;

bool AppConfig::HexSendUdpServer = false;
bool AppConfig::HexReceiveUdpServer = false;
bool AppConfig::AsciiUdpServer = false;
bool AppConfig::DebugUdpServer = false;
bool AppConfig::AutoSendUdpServer = false;
int AppConfig::IntervalUdpServer = 1000;
QString AppConfig::UdpListenIP = "127.0.0.1";
int AppConfig::UdpListenPort = 6000;
bool AppConfig::SelectAllUdpServer = true;

bool AppConfig::HexSendWebClient = false;
bool AppConfig::HexReceiveWebClient = false;
bool AppConfig::AsciiWebClient = true;
bool AppConfig::DebugWebClient = false;
bool AppConfig::AutoSendWebClient = false;
int AppConfig::IntervalWebClient = 1000;
QString AppConfig::WebServerIP = "ws://127.0.0.1";
int AppConfig::WebServerPort = 6001;

bool AppConfig::HexSendWebServer = false;
bool AppConfig::HexReceiveWebServer = false;
bool AppConfig::AsciiWebServer = true;
bool AppConfig::DebugWebServer = false;
bool AppConfig::AutoSendWebServer = false;
int AppConfig::IntervalWebServer = 1000;
QString AppConfig::WebListenIP = "127.0.0.1";
int AppConfig::WebListenPort = 6001;
bool AppConfig::SelectAllWebServer = true;

void AppConfig::readConfig()
{    
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    AppConfig::CurrentIndex = set.value("CurrentIndex").toInt();
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    AppConfig::HexSendTcpClient = set.value("HexSendTcpClient", AppConfig::HexSendTcpClient).toBool();
    AppConfig::HexReceiveTcpClient = set.value("HexReceiveTcpClient", AppConfig::HexReceiveTcpClient).toBool();
    AppConfig::AsciiTcpClient = set.value("AsciiTcpClient", AppConfig::AsciiTcpClient).toBool();
    AppConfig::DebugTcpClient = set.value("DebugTcpClient", AppConfig::DebugTcpClient).toBool();
    AppConfig::AutoSendTcpClient = set.value("AutoSendTcpClient", AppConfig::AutoSendTcpClient).toBool();
    AppConfig::IntervalTcpClient = set.value("IntervalTcpClient", AppConfig::IntervalTcpClient).toInt();
    AppConfig::TcpBindIP = set.value("TcpBindIP", AppConfig::TcpBindIP).toString();
    AppConfig::TcpBindPort = set.value("TcpBindPort", AppConfig::TcpBindPort).toInt();
    AppConfig::TcpServerIP = set.value("TcpServerIP", AppConfig::TcpServerIP).toString();
    AppConfig::TcpServerPort = set.value("TcpServerPort", AppConfig::TcpServerPort).toInt();
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    AppConfig::HexSendTcpServer = set.value("HexSendTcpServer", AppConfig::HexSendTcpServer).toBool();
    AppConfig::HexReceiveTcpServer = set.value("HexReceiveTcpServer", AppConfig::HexReceiveTcpServer).toBool();
    AppConfig::AsciiTcpServer = set.value("AsciiTcpServer", AppConfig::AsciiTcpServer).toBool();
    AppConfig::DebugTcpServer = set.value("DebugTcpServer", AppConfig::DebugTcpServer).toBool();
    AppConfig::AutoSendTcpServer = set.value("AutoSendTcpServer", AppConfig::AutoSendTcpServer).toBool();
    AppConfig::IntervalTcpServer = set.value("IntervalTcpServer", AppConfig::IntervalTcpServer).toInt();
    AppConfig::TcpListenIP = set.value("TcpListenIP", AppConfig::TcpListenIP).toString();
    AppConfig::TcpListenPort = set.value("TcpListenPort", AppConfig::TcpListenPort).toInt();
    AppConfig::SelectAllTcpServer = set.value("SelectAllTcpServer", AppConfig::SelectAllTcpServer).toBool();
    set.endGroup();

    set.beginGroup("UdpClientConfig");
    AppConfig::HexSendUdpClient = set.value("HexSendUdpClient", AppConfig::HexSendUdpClient).toBool();
    AppConfig::HexReceiveUdpClient = set.value("HexReceiveUdpClient", AppConfig::HexReceiveUdpClient).toBool();
    AppConfig::AsciiUdpClient = set.value("AsciiUdpClient", AppConfig::AsciiUdpClient).toBool();
    AppConfig::DebugUdpClient = set.value("DebugUdpClient", AppConfig::DebugUdpClient).toBool();
    AppConfig::AutoSendUdpClient = set.value("AutoSendUdpClient", AppConfig::AutoSendUdpClient).toBool();
    AppConfig::IntervalUdpClient = set.value("IntervalUdpClient", AppConfig::IntervalUdpClient).toInt();
    AppConfig::UdpBindIP = set.value("UdpBindIP", AppConfig::UdpBindIP).toString();
    AppConfig::UdpBindPort = set.value("UdpBindPort", AppConfig::UdpBindPort).toInt();
    AppConfig::UdpServerIP = set.value("UdpServerIP", AppConfig::UdpServerIP).toString();
    AppConfig::UdpServerPort = set.value("UdpServerPort", AppConfig::UdpServerPort).toInt();
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    AppConfig::HexSendUdpServer = set.value("HexSendUdpServer", AppConfig::HexSendUdpServer).toBool();
    AppConfig::HexReceiveUdpServer = set.value("HexReceiveUdpServer", AppConfig::HexReceiveUdpServer).toBool();
    AppConfig::AsciiUdpServer = set.value("AsciiUdpServer", AppConfig::AsciiUdpServer).toBool();
    AppConfig::DebugUdpServer = set.value("DebugUdpServer", AppConfig::DebugUdpServer).toBool();
    AppConfig::AutoSendUdpServer = set.value("AutoSendUdpServer", AppConfig::AutoSendUdpServer).toBool();
    AppConfig::IntervalUdpServer = set.value("IntervalUdpServer", AppConfig::IntervalUdpServer).toInt();
    AppConfig::UdpListenIP = set.value("UdpListenIP", AppConfig::UdpListenIP).toString();
    AppConfig::UdpListenPort = set.value("UdpListenPort", AppConfig::UdpListenPort).toInt();
    AppConfig::SelectAllUdpServer = set.value("SelectAllUdpServer", AppConfig::SelectAllUdpServer).toBool();
    set.endGroup();

    set.beginGroup("WebClientConfig");
    AppConfig::HexSendWebClient = set.value("HexSendWebClient", AppConfig::HexSendWebClient).toBool();
    AppConfig::HexReceiveWebClient = set.value("HexReceiveWebClient", AppConfig::HexReceiveWebClient).toBool();
    AppConfig::AsciiWebClient = set.value("AsciiWebClient", AppConfig::AsciiWebClient).toBool();
    AppConfig::DebugWebClient = set.value("DebugWebClient", AppConfig::DebugWebClient).toBool();
    AppConfig::AutoSendWebClient = set.value("AutoSendWebClient", AppConfig::AutoSendWebClient).toBool();
    AppConfig::IntervalWebClient = set.value("IntervalWebClient", AppConfig::IntervalWebClient).toInt();
    AppConfig::WebServerIP = set.value("WebServerIP", AppConfig::WebServerIP).toString();
    AppConfig::WebServerPort = set.value("WebServerPort", AppConfig::WebServerPort).toInt();
    set.endGroup();

    set.beginGroup("WebServerConfig");
    AppConfig::HexSendWebServer = set.value("HexSendWebServer", AppConfig::HexSendWebServer).toBool();
    AppConfig::HexReceiveWebServer = set.value("HexReceiveWebServer", AppConfig::HexReceiveWebServer).toBool();
    AppConfig::AsciiWebServer = set.value("AsciiWebServer", AppConfig::AsciiWebServer).toBool();
    AppConfig::DebugWebServer = set.value("DebugWebServer", AppConfig::DebugWebServer).toBool();
    AppConfig::AutoSendWebServer = set.value("AutoSendWebServer", AppConfig::AutoSendWebServer).toBool();
    AppConfig::IntervalWebServer = set.value("IntervalWebServer", AppConfig::IntervalWebServer).toInt();
    AppConfig::WebListenIP = set.value("WebListenIP", AppConfig::WebListenIP).toString();
    AppConfig::WebListenPort = set.value("WebListenPort", AppConfig::WebListenPort).toInt();
    AppConfig::SelectAllWebServer = set.value("SelectAllWebServer", AppConfig::SelectAllWebServer).toBool();
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

    set.beginGroup("AppConfig");
    set.setValue("CurrentIndex", AppConfig::CurrentIndex);
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    set.setValue("HexSendTcpClient", AppConfig::HexSendTcpClient);
    set.setValue("HexReceiveTcpClient", AppConfig::HexReceiveTcpClient);
    set.setValue("DebugTcpClient", AppConfig::DebugTcpClient);
    set.setValue("AutoSendTcpClient", AppConfig::AutoSendTcpClient);
    set.setValue("IntervalTcpClient", AppConfig::IntervalTcpClient);
    set.setValue("TcpBindIP", AppConfig::TcpBindIP);
    set.setValue("TcpBindPort", AppConfig::TcpBindPort);
    set.setValue("TcpServerIP", AppConfig::TcpServerIP);
    set.setValue("TcpServerPort", AppConfig::TcpServerPort);
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    set.setValue("HexSendTcpServer", AppConfig::HexSendTcpServer);
    set.setValue("HexReceiveTcpServer", AppConfig::HexReceiveTcpServer);
    set.setValue("DebugTcpServer", AppConfig::DebugTcpServer);
    set.setValue("AutoSendTcpServer", AppConfig::AutoSendTcpServer);
    set.setValue("IntervalTcpServer", AppConfig::IntervalTcpServer);
    set.setValue("TcpListenIP", AppConfig::TcpListenIP);
    set.setValue("TcpListenPort", AppConfig::TcpListenPort);
    set.setValue("SelectAllTcpServer", AppConfig::SelectAllTcpServer);
    set.endGroup();

    set.beginGroup("UdpClientConfig");
    set.setValue("HexSendUdpClient", AppConfig::HexSendUdpClient);
    set.setValue("HexReceiveUdpClient", AppConfig::HexReceiveUdpClient);
    set.setValue("DebugUdpClient", AppConfig::DebugUdpClient);
    set.setValue("AutoSendUdpClient", AppConfig::AutoSendUdpClient);
    set.setValue("IntervalUdpClient", AppConfig::IntervalUdpClient);
    set.setValue("UdpBindIP", AppConfig::UdpBindIP);
    set.setValue("UdpBindPort", AppConfig::UdpBindPort);
    set.setValue("UdpServerIP", AppConfig::UdpServerIP);
    set.setValue("UdpServerPort", AppConfig::UdpServerPort);
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    set.setValue("HexSendUdpServer", AppConfig::HexSendUdpServer);
    set.setValue("HexReceiveUdpServer", AppConfig::HexReceiveUdpServer);
    set.setValue("DebugUdpServer", AppConfig::DebugUdpServer);
    set.setValue("AutoSendUdpServer", AppConfig::AutoSendUdpServer);
    set.setValue("IntervalUdpServer", AppConfig::IntervalUdpServer);
    set.setValue("UdpListenIP", AppConfig::UdpListenIP);
    set.setValue("UdpListenPort", AppConfig::UdpListenPort);
    set.setValue("SelectAllUdpServer", AppConfig::SelectAllUdpServer);
    set.endGroup();

    set.beginGroup("WebClientConfig");
    set.setValue("HexSendWebClient", AppConfig::HexSendWebClient);
    set.setValue("HexReceiveWebClient", AppConfig::HexReceiveWebClient);
    set.setValue("DebugWebClient", AppConfig::DebugWebClient);
    set.setValue("AutoSendWebClient", AppConfig::AutoSendWebClient);
    set.setValue("IntervalWebClient", AppConfig::IntervalWebClient);
    set.setValue("WebServerIP", AppConfig::WebServerIP);
    set.setValue("WebServerPort", AppConfig::WebServerPort);
    set.endGroup();

    set.beginGroup("WebServerConfig");
    set.setValue("HexSendWebServer", AppConfig::HexSendWebServer);
    set.setValue("HexReceiveWebServer", AppConfig::HexReceiveWebServer);
    set.setValue("DebugWebServer", AppConfig::DebugWebServer);
    set.setValue("AutoSendWebServer", AppConfig::AutoSendWebServer);
    set.setValue("IntervalWebServer", AppConfig::IntervalWebServer);
    set.setValue("WebListenIP", AppConfig::WebListenIP);
    set.setValue("WebListenPort", AppConfig::WebListenPort);
    set.setValue("SelectAllWebServer", AppConfig::SelectAllWebServer);
    set.endGroup();
}
