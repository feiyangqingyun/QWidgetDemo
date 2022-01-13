#include "frmmain.h"
#include "head.h"

int main(int argc, char *argv[])
{
    //设置不应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

    QApplication a(argc, argv);
    QFont font;
    font.setFamily("Microsoft Yahei");
    font.setPixelSize(13);
    a.setFont(font);

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
    w.setWindowTitle("样式表示例 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();    

    return a.exec();
}
