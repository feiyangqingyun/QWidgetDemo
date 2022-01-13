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

    //设置编码以及加载中文翻译文件
    QUIHelper::initAll();

    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();

    frmMain w;
    w.setWindowTitle("网络中转服务器 V2022 (QQ: 517216493 WX: feiyangqingyun)");
    w.resize(900, 650);
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
