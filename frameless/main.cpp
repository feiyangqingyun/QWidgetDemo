#include "quihelper.h"
#include "mainwindow.h"
#include "widget.h"
#include "dialog.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
    QApplication a(argc, argv);

    QUIHelper::setStyle();
    QUIHelper::setFont(15);
    QUIHelper::setCode();

    MainWindow w;
    //Widget w;
    //Dialog w;

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    w.resize(800, 600);
#else
    w.resize(800, 550);
#endif
    QUIHelper::setFormInCenter(&w);
    w.show();
    return a.exec();
}
