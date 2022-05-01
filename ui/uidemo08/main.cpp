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
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
