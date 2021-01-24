#include "frmcomtool.h"
#include "quiwidget.h"

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

    App::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    App::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    App::readConfig();
    App::readSendData();
    App::readDeviceData();

    frmComTool w;
    w.setWindowTitle("串口调试助手 V2021 (QQ: 517216493 WX: feiyangqingyun)");
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
