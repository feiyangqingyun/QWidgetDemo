#pragma execution_character_set("utf-8")

#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.setWindowTitle("异形窗体 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();

    return a.exec();
}
