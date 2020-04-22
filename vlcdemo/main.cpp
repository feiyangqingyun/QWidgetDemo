#pragma execution_character_set("utf-8")
#include "widget.h"

#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPixelSize(12);
    a.setFont(font);

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
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

    Widget w;
    w.setWindowTitle("视频流播放vlc内核 (QQ: 517216493)");
    w.show();

    //居中显示窗体
    QDesktopWidget deskWidget;
    int deskWidth = deskWidget.availableGeometry().width();
    int deskHeight = deskWidget.availableGeometry().height();
    QPoint movePoint(deskWidth / 2 - w.width() / 2, deskHeight / 2 - w.height() / 2);
    w.move(movePoint);

    return a.exec();
}
