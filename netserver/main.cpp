#include "frmmain.h"
#include "quihelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    QFont font;
    font.setFamily(QUIConfig::FontName);
    font.setPixelSize(QUIConfig::FontSize);
    a.setFont(font);

    //设置编码以及加载中文翻译文件
    QUIHelper::setCode();
    QUIHelper::setTranslator(":/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/widgets.qm");
    QUIHelper::initRand();

    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();

    frmMain w;
    w.setWindowTitle(QString("网络中转服务器V2018  本机IP: %1  QQ: 517216493").arg(QUIHelper::getLocalIP()));
    w.show();

    return a.exec();
}
