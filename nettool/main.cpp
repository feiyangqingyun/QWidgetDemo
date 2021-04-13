#include "frmmain.h"
#include "quiwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    //设置编码+字体+中文翻译文件
    QUIHelper::setCode();
    QUIHelper::setFont(":/DroidSansFallback.ttf");
    QUIHelper::setTranslator(":/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/widgets.qm");
    QUIHelper::initRand();

    AppConfig::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();
    AppConfig::readSendData();
    AppConfig::readDeviceData();

    frmMain w;
    w.setWindowTitle("网络调试助手 V2021 (QQ: 517216493 WX: feiyangqingyun)");
    w.resize(950, 700);
    QUIHelper::setFormInCenter(&w);
    w.show();    

    return a.exec();
}
