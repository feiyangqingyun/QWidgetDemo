#pragma execution_character_set("utf-8")

#include "frmdbpage.h"
#include "qapplication.h"
#include "qtextcodec.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
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

    //打开数据库,整个应用程序可用
    QSqlDatabase dbConn = QSqlDatabase::addDatabase("QSQLITE");
    dbConn.setDatabaseName(qApp->applicationDirPath() + "/TA.db");
    if (dbConn.open()) {
        qDebug() << "连接数据库成功!";
    } else {
        qDebug() << "连接数据库失败!";
    }

    frmDbPage w;
    w.setWindowTitle("数据库分页示例 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();

    return a.exec();
}
