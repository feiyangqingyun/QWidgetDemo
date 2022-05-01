#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "iconhelper.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();   
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    int iconSize = 100;
    int iconWidth = 150;
    int iconHeight = 130;

    QList<QString> listColorBg;
    listColorBg << "#FF3739" << "#1A9FE0" << "#41BB1A" << "#1570A5" << "#FE781F" << "#9B59BB";
    QList<QString> listColorText;
    listColorText << "#FEFEFE" << "#FEFEFE" << "#FEFEFE" << "#FEFEFE" << "#FEFEFE" << "#FEFEFE";

    QList<int> icons;
    icons << 0xf2ba << 0xf002 << 0xf2c2 << 0xf02f << 0xf013 << 0xf021;
    QList<QString> names;
    names << "访客登记" << "记录查询" << "证件扫描" << "信息打印" << "系统设置" << "系统重启";

    QList<QToolButton *> btns = this->findChildren<QToolButton *>();
    for (int i = 0; i < btns.count(); ++i) {
        QToolButton *btn = btns.at(i);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setIconSize(QSize(iconWidth, iconHeight));

        QPixmap pix = IconHelper::getPixmap(listColorText.at(i), icons.at(i), iconSize, iconWidth, iconHeight);
        btn->setIcon(QIcon(pix));
        btn->setText(names.at(i));
        btn->setStyleSheet(QString("QToolButton{font:%1px;color:%2;background-color:%3;border:none;border-radius:0px;}")
                           .arg(iconSize / 2).arg(listColorText.at(i)).arg(listColorBg.at(i)));

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
    }
}

void frmMain::buttonClicked()
{
    QToolButton *btn = (QToolButton *)sender();
    QString text = btn->text();
    qDebug() << text;

    if (text == "系统重启") {
        close();
    }
}
