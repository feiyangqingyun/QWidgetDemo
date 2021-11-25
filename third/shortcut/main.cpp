#pragma execution_character_set("utf-8")

#include "frmshortcut.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    frmShortCut w;
    w.setWindowTitle("全局热键示例 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();

    return a.exec();
}
