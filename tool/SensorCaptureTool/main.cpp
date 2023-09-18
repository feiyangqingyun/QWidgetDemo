#include "frmcomtool.h"
#include "quihelper.h"

#define APP_VERSION_MAIN            (0)
#define APP_VERSION_MINOR           (1)
#define APP_VERSION_FIX             (0)
#define _WIN32_WINNT 0x0600

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

    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    frmComTool w;
    QString title = QString("%1 V%2.%3.%4").arg(QUIHelper::appName()).arg(APP_VERSION_MAIN).arg(APP_VERSION_MINOR).arg(APP_VERSION_FIX);
    w.setWindowTitle(title);
    w.resize(900, 650);
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
