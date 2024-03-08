#include "frmmain.h"
#include "appinit.h"
#include "qthelper.h"

int main(int argc, char *argv[])
{
    QtHelper::initMain();
    QApplication a(argc, argv);
    AppInit::Instance()->start();

    QtHelper::setFont();
    QtHelper::setCode();

    frmMain w;
    w.setWindowTitle("metro风格主界面 (QQ: 517216493 WX: feiyangqingyun)");
    QtHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
