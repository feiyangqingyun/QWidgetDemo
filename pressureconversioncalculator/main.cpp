#include "frmmain.h"
#include "appinit.h"
#include "qapplication.h"
#include "qtextcodec.h"
#include "qfile.h"
#include "qdebug.h"

/* 压力单位转换工具
 * 10种以上不同压力单位间的快速转换(具有转换的单位,以及转换的系数)
 * 实时单位转换
 * 小数点保留位可设(1-10)
 * 所有压力单位同时显示在界面上
 * 不需要转换按钮输入时,输入实时转换
 * 检查输入是否合法
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));
    AppInit::Instance()->start();

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

    //加载样式表
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }

    frmMain w;
    w.show();

    return a.exec();
}

