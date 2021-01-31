#include "app.h"
#include "quiwidget.h"

QString App::ConfigFile = "config.ini";
QString App::SendFileName = "send.txt";
QString App::DeviceFileName = "device.txt";

int App::CurrentIndex = 0;

bool App::HexSendTcpClient = false;
bool App::HexReceiveTcpClient = false;
bool App::AsciiTcpClient = false;
bool App::DebugTcpClient = false;
bool App::AutoSendTcpClient = false;
int App::IntervalTcpClient = 1000;
QString App::TcpServerIP = "127.0.0.1";
int App::TcpServerPort = 6000;

bool App::HexSendTcpServer = false;
bool App::HexReceiveTcpServer = false;
bool App::AsciiTcpServer = false;
bool App::DebugTcpServer = false;
bool App::AutoSendTcpServer = false;
int App::IntervalTcpServer = 1000;
QString App::TcpListenIP = "127.0.0.1";
int App::TcpListenPort = 6000;
bool App::SelectAllTcpServer = true;

bool App::HexSendUdpClient = false;
bool App::HexReceiveUdpClient = false;
bool App::AsciiUdpClient = false;
bool App::DebugUdpClient = false;
bool App::AutoSendUdpClient = false;
int App::IntervalUdpClient = 1000;
QString App::UdpServerIP = "127.0.0.1";
int App::UdpServerPort = 6000;

bool App::HexSendUdpServer = false;
bool App::HexReceiveUdpServer = false;
bool App::AsciiUdpServer = false;
bool App::DebugUdpServer = false;
bool App::AutoSendUdpServer = false;
int App::IntervalUdpServer = 1000;
QString App::UdpListenIP = "127.0.0.1";
int App::UdpListenPort = 6000;
bool App::SelectAllUdpServer = false;

bool App::HexSendWebClient = false;
bool App::HexReceiveWebClient = false;
bool App::AsciiWebClient = true;
bool App::DebugWebClient = false;
bool App::AutoSendWebClient = false;
int App::IntervalWebClient = 1000;
QString App::WebServerIP = "ws://127.0.0.1";
int App::WebServerPort = 6000;

bool App::HexSendWebServer = false;
bool App::HexReceiveWebServer = false;
bool App::AsciiWebServer = true;
bool App::DebugWebServer = false;
bool App::AutoSendWebServer = false;
int App::IntervalWebServer = 1000;
QString App::WebListenIP = "127.0.0.1";
int App::WebListenPort = 6000;
bool App::SelectAllWebServer = true;

void App::readConfig()
{
    if (!QUIHelper::checkIniFile(App::ConfigFile)) {
        writeConfig();
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    App::CurrentIndex = set.value("CurrentIndex").toInt();
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    App::HexSendTcpClient = set.value("HexSendTcpClient", App::HexSendTcpClient).toBool();
    App::HexReceiveTcpClient = set.value("HexReceiveTcpClient", App::HexReceiveTcpClient).toBool();
    App::AsciiTcpClient = set.value("AsciiTcpClient", App::AsciiTcpClient).toBool();
    App::DebugTcpClient = set.value("DebugTcpClient", App::DebugTcpClient).toBool();
    App::AutoSendTcpClient = set.value("AutoSendTcpClient", App::AutoSendTcpClient).toBool();
    App::IntervalTcpClient = set.value("IntervalTcpClient", App::IntervalTcpClient).toInt();
    App::TcpServerIP = set.value("TcpServerIP", App::TcpServerIP).toString();
    App::TcpServerPort = set.value("TcpServerPort", App::TcpServerPort).toInt();
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    App::HexSendTcpServer = set.value("HexSendTcpServer", App::HexSendTcpServer).toBool();
    App::HexReceiveTcpServer = set.value("HexReceiveTcpServer", App::HexReceiveTcpServer).toBool();
    App::AsciiTcpServer = set.value("AsciiTcpServer", App::AsciiTcpServer).toBool();
    App::DebugTcpServer = set.value("DebugTcpServer", App::DebugTcpServer).toBool();
    App::AutoSendTcpServer = set.value("AutoSendTcpServer", App::AutoSendTcpServer).toBool();
    App::IntervalTcpServer = set.value("IntervalTcpServer", App::IntervalTcpServer).toInt();
    App::TcpListenIP = set.value("TcpListenIP", App::TcpListenIP).toString();
    App::TcpListenPort = set.value("TcpListenPort", App::TcpListenPort).toInt();
    App::SelectAllTcpServer = set.value("SelectAllTcpServer", App::SelectAllTcpServer).toBool();
    set.endGroup();

    set.beginGroup("UdpClientConfig");
    App::HexSendUdpClient = set.value("HexSendUdpClient", App::HexSendUdpClient).toBool();
    App::HexReceiveUdpClient = set.value("HexReceiveUdpClient", App::HexReceiveUdpClient).toBool();
    App::AsciiUdpClient = set.value("AsciiUdpClient", App::AsciiUdpClient).toBool();
    App::DebugUdpClient = set.value("DebugUdpClient", App::DebugUdpClient).toBool();
    App::AutoSendUdpClient = set.value("AutoSendUdpClient", App::AutoSendUdpClient).toBool();
    App::IntervalUdpClient = set.value("IntervalUdpClient", App::IntervalUdpClient).toInt();
    App::UdpServerIP = set.value("UdpServerIP", App::UdpServerIP).toString();
    App::UdpServerPort = set.value("UdpServerPort", App::UdpServerPort).toInt();
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    App::HexSendUdpServer = set.value("HexSendUdpServer", App::HexSendUdpServer).toBool();
    App::HexReceiveUdpServer = set.value("HexReceiveUdpServer", App::HexReceiveUdpServer).toBool();
    App::AsciiUdpServer = set.value("AsciiUdpServer", App::AsciiUdpServer).toBool();
    App::DebugUdpServer = set.value("DebugUdpServer", App::DebugUdpServer).toBool();
    App::AutoSendUdpServer = set.value("AutoSendUdpServer", App::AutoSendUdpServer).toBool();
    App::IntervalUdpServer = set.value("IntervalUdpServer", App::IntervalUdpServer).toInt();
    App::UdpListenIP = set.value("UdpListenIP", App::UdpListenIP).toString();
    App::UdpListenPort = set.value("UdpListenPort", App::UdpListenPort).toInt();
    App::SelectAllUdpServer = set.value("SelectAllUdpServer", App::SelectAllUdpServer).toBool();
    set.endGroup();

    set.beginGroup("WebClientConfig");
    App::HexSendWebClient = set.value("HexSendWebClient", App::HexSendWebClient).toBool();
    App::HexReceiveWebClient = set.value("HexReceiveWebClient", App::HexReceiveWebClient).toBool();
    App::AsciiWebClient = set.value("AsciiWebClient", App::AsciiWebClient).toBool();
    App::DebugWebClient = set.value("DebugWebClient", App::DebugWebClient).toBool();
    App::AutoSendWebClient = set.value("AutoSendWebClient", App::AutoSendWebClient).toBool();
    App::IntervalWebClient = set.value("IntervalWebClient", App::IntervalWebClient).toInt();
    App::WebServerIP = set.value("WebServerIP", App::WebServerIP).toString();
    App::WebServerPort = set.value("WebServerPort", App::WebServerPort).toInt();
    set.endGroup();

    set.beginGroup("WebServerConfig");
    App::HexSendWebServer = set.value("HexSendWebServer", App::HexSendWebServer).toBool();
    App::HexReceiveWebServer = set.value("HexReceiveWebServer", App::HexReceiveWebServer).toBool();
    App::AsciiWebServer = set.value("AsciiWebServer", App::AsciiWebServer).toBool();
    App::DebugWebServer = set.value("DebugWebServer", App::DebugWebServer).toBool();
    App::AutoSendWebServer = set.value("AutoSendWebServer", App::AutoSendWebServer).toBool();
    App::IntervalWebServer = set.value("IntervalWebServer", App::IntervalWebServer).toInt();
    App::WebListenIP = set.value("WebListenIP", App::WebListenIP).toString();
    App::WebListenPort = set.value("WebListenPort", App::WebListenPort).toInt();
    App::SelectAllWebServer = set.value("SelectAllWebServer", App::SelectAllWebServer).toBool();
    set.endGroup();
}

void App::writeConfig()
{
    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    set.setValue("CurrentIndex", App::CurrentIndex);
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    set.setValue("HexSendTcpClient", App::HexSendTcpClient);
    set.setValue("HexReceiveTcpClient", App::HexReceiveTcpClient);
    set.setValue("DebugTcpClient", App::DebugTcpClient);
    set.setValue("AutoSendTcpClient", App::AutoSendTcpClient);
    set.setValue("IntervalTcpClient", App::IntervalTcpClient);
    set.setValue("TcpServerIP", App::TcpServerIP);
    set.setValue("TcpServerPort", App::TcpServerPort);
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    set.setValue("HexSendTcpServer", App::HexSendTcpServer);
    set.setValue("HexReceiveTcpServer", App::HexReceiveTcpServer);
    set.setValue("DebugTcpServer", App::DebugTcpServer);
    set.setValue("AutoSendTcpServer", App::AutoSendTcpServer);
    set.setValue("IntervalTcpServer", App::IntervalTcpServer);
    set.setValue("TcpListenIP", App::TcpListenIP);
    set.setValue("TcpListenPort", App::TcpListenPort);
    set.setValue("SelectAllTcpServer", App::SelectAllTcpServer);
    set.endGroup();

    set.beginGroup("UdpClientConfig");
    set.setValue("HexSendUdpClient", App::HexSendUdpClient);
    set.setValue("HexReceiveUdpClient", App::HexReceiveUdpClient);
    set.setValue("DebugUdpClient", App::DebugUdpClient);
    set.setValue("AutoSendUdpClient", App::AutoSendUdpClient);
    set.setValue("IntervalUdpClient", App::IntervalUdpClient);
    set.setValue("UdpServerIP", App::UdpServerIP);
    set.setValue("UdpServerPort", App::UdpServerPort);
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    set.setValue("HexSendUdpServer", App::HexSendUdpServer);
    set.setValue("HexReceiveUdpServer", App::HexReceiveUdpServer);
    set.setValue("DebugUdpServer", App::DebugUdpServer);
    set.setValue("AutoSendUdpServer", App::AutoSendUdpServer);
    set.setValue("IntervalUdpServer", App::IntervalUdpServer);
    set.setValue("UdpListenIP", App::UdpListenIP);
    set.setValue("UdpListenPort", App::UdpListenPort);
    set.setValue("SelectAllUdpServer", App::SelectAllUdpServer);
    set.endGroup();

    set.beginGroup("WebClientConfig");
    set.setValue("HexSendWebClient", App::HexSendWebClient);
    set.setValue("HexReceiveWebClient", App::HexReceiveWebClient);
    set.setValue("DebugWebClient", App::DebugWebClient);
    set.setValue("AutoSendWebClient", App::AutoSendWebClient);
    set.setValue("IntervalWebClient", App::IntervalWebClient);
    set.setValue("WebServerIP", App::WebServerIP);
    set.setValue("WebServerPort", App::WebServerPort);
    set.endGroup();

    set.beginGroup("WebServerConfig");
    set.setValue("HexSendWebServer", App::HexSendWebServer);
    set.setValue("HexReceiveWebServer", App::HexReceiveWebServer);
    set.setValue("DebugWebServer", App::DebugWebServer);
    set.setValue("AutoSendWebServer", App::AutoSendWebServer);
    set.setValue("IntervalWebServer", App::IntervalWebServer);
    set.setValue("WebListenIP", App::WebListenIP);
    set.setValue("WebListenPort", App::WebListenPort);
    set.setValue("SelectAllWebServer", App::SelectAllWebServer);
    set.endGroup();
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

    if (App::Datas.count() == 0) {
        App::Datas << "16 FF 01 01 E0 E1" << "16 FF 01 01 E1 E2";
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

void App::saveData(const QString &data)
{
    if (data.length() <= 0) {
        return;
    }

    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(STRDATETIME);
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        file.write(data.toUtf8());
        file.close();
    }
}
