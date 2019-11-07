#include "frmmain.h"
#include "qapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    frmMain w;
    w.setWindowTitle("qwtdemo");
    w.show();

    return a.exec();
}
