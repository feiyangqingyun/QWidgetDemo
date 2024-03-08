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
    //加载样式表
    QtHelper::setStyle(":/qss/blacksoft.css");

    frmMain w;
    QtHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
