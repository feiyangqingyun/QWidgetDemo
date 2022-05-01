#include "frmmain.h"
#include "appinit.h"
#include "quihelper.h"

int main(int argc, char *argv[])
{
    QUIHelper::initMain();
    QApplication a(argc, argv);
    AppInit::Instance()->start();

    QUIHelper::setFont();
    QUIHelper::setCode();

    frmMain w;
    w.setWindowTitle("metro风格主界面 (QQ: 517216493 WX: feiyangqingyun)");
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
