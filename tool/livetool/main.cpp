#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "trayicon.h"
#include "app.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif

    frmMain w;
    w.setWindowTitle("程序启动器 (QQ: 517216493 WX: feiyangqingyun)");
    w.setFixedSize(w.sizeHint());

    //启动托盘类
    TrayIcon::Instance()->setMainWidget(&w);
    TrayIcon::Instance()->setIcon(":/main.ico");
    TrayIcon::Instance()->setToolTip(w.windowTitle());
    TrayIcon::Instance()->setVisible(true);
    QObject::connect(&w, SIGNAL(destroyed(QObject *)), TrayIcon::Instance(), SLOT(closeAll()));

    App::ConfigFile = qApp->applicationDirPath() + "/config.ini";
    App::readConfig();
    App::TargetAppName.isEmpty() ? w.show() : w.hide();

    return a.exec();
}
