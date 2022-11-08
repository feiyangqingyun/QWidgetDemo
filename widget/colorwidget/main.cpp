﻿#pragma execution_character_set("utf-8")

#include "frmcolorwidget.h"
#include <QApplication>
#include <QTextCodec>
//test
int test;
int test2;
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

    frmColorWidget w;
    w.setWindowTitle("屏幕拾色器 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();

    return a.exec();
}
