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

    //加载样式表
    QFile file(":/qss/blacksoft.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }

    frmMain w;
    w.resize(800, 600);
    QUIHelper::setFormInCenter(&w);
    w.show();

    return a.exec();
}
