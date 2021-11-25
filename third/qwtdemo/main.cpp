#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "qapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    frmMain w;
    w.setWindowTitle("qwt无插件示例 (QQ: 517216493 WX: feiyangqingyun)");
    w.show();

    return a.exec();
}
