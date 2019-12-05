#ifndef APP_H
#define APP_H

#include "head.h"

class App
{
public:
    static QString ConfigFile;          //配置文件路径
    static QString SendFileName;        //发送配置文件名
    static QString DeviceFileName;      //模拟设备数据文件名

    static int CurrentIndex;            //当前索引

    //TCP客户端配置参数
    static bool HexSendTcpClient;       //16进制发送
    static bool HexReceiveTcpClient;    //16进制接收
    static bool AsciiTcpClient;         //ASCII模式
    static bool DebugTcpClient;         //启用数据调试
    static bool AutoSendTcpClient;      //自动发送数据
    static int IntervalTcpClient;       //发送数据间隔
    static QString TcpServerIP;         //服务器IP
    static int TcpServerPort;           //服务器端口

    //TCP服务器配置参数
    static bool HexSendTcpServer;       //16进制发送
    static bool HexReceiveTcpServer;    //16进制接收
    static bool AsciiTcpServer;         //ASCII模式
    static bool DebugTcpServer;         //启用数据调试
    static bool AutoSendTcpServer;      //自动发送数据
    static int IntervalTcpServer;       //发送数据间隔    
    static QString TcpListenIP;         //监听地址
    static int TcpListenPort;           //监听端口
    static bool SelectAllTcpServer;     //选中所有

    //UDP客户端配置参数
    static bool HexSendUdpClient;       //16进制发送
    static bool HexReceiveUdpClient;    //16进制接收
    static bool AsciiUdpClient;         //ASCII模式
    static bool DebugUdpClient;         //启用数据调试
    static bool AutoSendUdpClient;      //自动发送数据
    static int IntervalUdpClient;       //发送数据间隔
    static QString UdpServerIP;         //服务器IP
    static int UdpServerPort;           //服务器端口

    //UDP服务器配置参数
    static bool HexSendUdpServer;       //16进制发送
    static bool HexReceiveUdpServer;    //16进制接收
    static bool AsciiUdpServer;         //ASCII模式
    static bool DebugUdpServer;         //启用数据调试
    static bool AutoSendUdpServer;      //自动发送数据
    static int IntervalUdpServer;       //发送数据间隔    
    static QString UdpListenIP;         //监听地址
    static int UdpListenPort;           //监听端口
    static bool SelectAllUdpServer;     //选中所有

    //读写配置参数及其他操作
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
    static void newConfig();            //以初始值新建配置文件
    static bool checkConfig();          //校验配置文件

    static QStringList Intervals;
    static QStringList Datas;
    static QStringList Keys;
    static QStringList Values;
    static void readSendData();
    static void readDeviceData();
};

#endif // APP_H
