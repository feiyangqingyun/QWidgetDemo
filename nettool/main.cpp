#include "frmmain.h"
#include "quiwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));    

    //设置编码以及加载中文翻译文件
    QUIHelper::setCode();
    QUIHelper::setFont(":/DroidSansFallback.ttf");
    QUIHelper::setTranslator(":/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/widgets.qm");
    QUIHelper::initRand();

    App::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    App::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    App::readConfig();
    App::readSendData();
    App::readDeviceData();

    frmMain w;
    w.setWindowTitle(QString("网络调试助手V2020 本机IP: %1 QQ: 517216493").arg(QUIHelper::getLocalIP()));
    w.show();

    return a.exec();
}
