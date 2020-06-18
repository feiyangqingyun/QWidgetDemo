#ifndef APP_H
#define APP_H

#include <QStringList>

class App
{
public:
    static QString ConfigFile;          //配置文件文件路径及名称
    static QString TargetAppName;       //目标软件程序名称
    static int TargetAppPort;           //目标软件通信端口
    static bool ReStartExplorer;        //是否需要重启桌面
    static int TimeoutCount;            //超时次数
    static int ReStartCount;            //已重启次数
    static QString ReStartLastTime;     //最后一次重启时间

    static void readConfig();           //读取配置文件,在main函数最开始加载程序载入
    static void writeConfig();          //写入配置文件,在更改配置文件程序关闭时调用
    static bool checkConfig();          //校验配置文件    

};

#endif // APP_H
