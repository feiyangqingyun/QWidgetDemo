#include "frmhotkey.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmHotKey w;
    w.show();
    return a.exec();
}
