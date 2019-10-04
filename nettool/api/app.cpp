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
bool App::SelectAllTcpServer = false;
int App::IntervalTcpServer = 1000;
int App::TcpListenPort = 6000;

bool App::HexSendUdpServer = false;
bool App::HexReceiveUdpServer = false;
bool App::AsciiUdpServer = false;
bool App::DebugUdpServer = false;
bool App::AutoSendUdpServer = false;
int App::IntervalUdpServer = 1000;
int App::UdpListenPort = 6000;
QString App::UdpServerIP = "127.0.0.1";
int App::UdpServerPort = 6000;

void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    App::CurrentIndex = set.value("CurrentIndex").toInt();
    set.endGroup();

    set.beginGroup("TcpClientConfig");
    App::HexSendTcpClient = set.value("HexSendTcpClient").toBool();
    App::HexReceiveTcpClient = set.value("HexReceiveTcpClient").toBool();
    App::AsciiTcpClient = set.value("AsciiTcpClient").toBool();
    App::DebugTcpClient = set.value("DebugTcpClient").toBool();
    App::AutoSendTcpClient = set.value("AutoSendTcpClient").toBool();
    App::IntervalTcpClient = set.value("IntervalTcpClient").toInt();
    App::TcpServerIP = set.value("TcpServerIP").toString();
    App::TcpServerPort = set.value("TcpServerPort").toInt();
    set.endGroup();

    set.beginGroup("TcpServerConfig");
    App::HexSendTcpServer = set.value("HexSendTcpServer").toBool();
    App::HexReceiveTcpServer = set.value("HexReceiveTcpServer").toBool();
    App::AsciiTcpServer = set.value("AsciiTcpServer").toBool();
    App::DebugTcpServer = set.value("DebugTcpServer").toBool();
    App::AutoSendTcpServer = set.value("AutoSendTcpServer").toBool();
    App::SelectAllTcpServer = set.value("SelectAllTcpServer").toBool();
    App::IntervalTcpServer = set.value("IntervalTcpServer").toInt();
    App::TcpListenPort = set.value("TcpListenPort").toInt();
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    App::HexSendUdpServer = set.value("HexSendUdpServer").toBool();
    App::HexReceiveUdpServer = set.value("HexReceiveUdpServer").toBool();
    App::AsciiUdpServer = set.value("AsciiUdpServer").toBool();
    App::DebugUdpServer = set.value("DebugUdpServer").toBool();
    App::AutoSendUdpServer = set.value("AutoSendUdpServer").toBool();
    App::IntervalUdpServer = set.value("IntervalUdpServer").toInt();
    App::UdpServerIP = set.value("UdpServerIP").toString();
    App::UdpServerPort = set.value("UdpServerPort").toInt();
    App::UdpListenPort = set.value("UdpListenPort").toInt();
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
    set.setValue("SelectAllTcpServer", App::SelectAllTcpServer);
    set.setValue("IntervalTcpServer", App::IntervalTcpServer);
    set.setValue("TcpListenPort", App::TcpListenPort);
    set.endGroup();

    set.beginGroup("UdpServerConfig");
    set.setValue("HexSendUdpServer", App::HexSendUdpServer);
    set.setValue("HexReceiveUdpServer", App::HexReceiveUdpServer);
    set.setValue("DebugUdpServer", App::DebugUdpServer);
    set.setValue("AutoSendUdpServer", App::AutoSendUdpServer);
    set.setValue("IntervalUdpServer", App::IntervalUdpServer);
    set.setValue("UdpServerIP", App::UdpServerIP);
    set.setValue("UdpServerPort", App::UdpServerPort);
    set.setValue("UdpListenPort", App::UdpListenPort);
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
