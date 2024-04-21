#include "frmmain.h"
#include "qthelper.h"

int main(int argc, char *argv[])
{
    QtHelper::initMain();
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    //设置编码以及加载中文翻译文件
    QtHelper::initAll();

    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QtHelper::appPath()).arg(QtHelper::appName());
    AppConfig::readConfig();

    frmMain w;
    w.setWindowTitle("网络中转服务器 V2024 (QQ: 517216493 WX: feiyangqingyun)");
    w.resize(900, 650);
    QtHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
