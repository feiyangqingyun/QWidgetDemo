#include "frmshortcut.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmShortCut w;
    w.show();
    return a.exec();
}
