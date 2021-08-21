#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径
    static int CurrentIndex;            //当前索引

    //TCP客户端配置参数
    static bool HexSendTcpClient;       //16进制发送
    static bool HexReceiveTcpClient;    //16进制接收
    static bool AsciiTcpClient;         //ASCII模式
    static bool DebugTcpClient;         //启用数据调试
    static bool AutoSendTcpClient;      //自动发送数据
    static int IntervalTcpClient;       //发送数据间隔
    static QString TcpBindIP;           //绑定地址
    static int TcpBindPort;             //绑定端口
    static QString TcpServerIP;         //服务器地址
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
    static QString UdpBindIP;           //绑定地址
    static int UdpBindPort;             //绑定端口
    static QString UdpServerIP;         //服务器地址
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

    //WEB客户端配置参数
    static bool HexSendWebClient;       //16进制发送
    static bool HexReceiveWebClient;    //16进制接收
    static bool AsciiWebClient;         //ASCII模式
    static bool DebugWebClient;         //启用数据调试
    static bool AutoSendWebClient;      //自动发送数据
    static int IntervalWebClient;       //发送数据间隔
    static QString WebServerIP;         //服务器地址
    static int WebServerPort;           //服务器端口

    //WEB服务器配置参数
    static bool HexSendWebServer;       //16进制发送
    static bool HexReceiveWebServer;    //16进制接收
    static bool AsciiWebServer;         //ASCII模式
    static bool DebugWebServer;         //启用数据调试
    static bool AutoSendWebServer;      //自动发送数据
    static int IntervalWebServer;       //发送数据间隔
    static QString WebListenIP;         //监听地址
    static int WebListenPort;           //监听端口
    static bool SelectAllWebServer;     //选中所有

    //读写配置参数
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
};

#endif // APPCONFIG_H
