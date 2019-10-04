#include "frmmain.h"
#include "qapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));

    frmMain w;
    w.setWindowTitle("styledemo   Author: feiyangqingyun@163.com   QQ: 517216493");
    w.show();

    return a.exec();
}
