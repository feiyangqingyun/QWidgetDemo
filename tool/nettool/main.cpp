#include "frmmain.h"
#include "quihelper.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    //设置编码+字体+中文翻译文件
    QUIHelper::initAll();

    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();

    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    frmMain w;
    w.setWindowTitle("网络调试助手 V2022 (QQ: 517216493 WX: feiyangqingyun)");
    w.resize(950, 700);
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
