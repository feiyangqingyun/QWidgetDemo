#pragma execution_character_set("utf-8")
#include "frmvideobox.h"
#include "ui_frmvideobox.h"
#include "videobox.h"
#include "qapplication.h"
#include "qevent.h"
#include "qlabel.h"
#include "qmenu.h"
#include "qcursor.h"
#include "qlist.h"
#include "qdebug.h"

frmVideoBox::frmVideoBox(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoBox)
{
    ui->setupUi(this);
    this->initForm();
}

frmVideoBox::~frmVideoBox()
{
    delete ui;
}

void frmVideoBox::initForm()
{
    //实例化子对象
    QWidgetList widgets;
    for (int i = 0; i < 64; ++i) {
        //这里用QLabel做演示可以改成自己的窗体类比如视频监控窗体
        QLabel *label = new QLabel;
        label->setFrameShape(QLabel::Box);
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        label->setText(QString("通道 %1").arg(i + 1, 2, 10, QChar('0')));
        widgets << label;
    }

    //实例化盒子
    box = new VideoBox(this);
    //关联信号槽
    connect(box, SIGNAL(layoutChanged(int,QString,bool)), this, SLOT(layoutChanged(int, QString, bool)));
    //可以改成 1_4/5_8/1_36 等
    box->setLayoutType("1_16");
    box->setLayout(ui->gridLayout);
    box->setWidgets(widgets, widgets);

    //box->setMenuFlag("排列");
    //box->setActionFlag("监控");
    //增加自定义布局(通道1开始3行3列布局)
    //box->appendType(1, 3, 4);

    //实例化菜单/先添加自己的菜单
    menu = new QMenu(this);
    //这里关联到一个槽函数处理,也可以关联到不同的槽函数
    menu->addAction("切换全屏模式", this, SLOT(doAction()));
    menu->addAction("启动轮询视频", this, SLOT(doAction()));
    menu->addSeparator();

    //把菜单加到盒子上
    box->initMenu(menu);
    box->show_all();
}

void frmVideoBox::doAction()
{
    //判断是哪个动作触发的
    QAction *action = (QAction *)sender();
    ui->label->setText(QString("触发了菜单： %1").arg(action->text()));
}

void frmVideoBox::layoutChanged(int type, const QString &layoutType, bool isMax)
{
    QString info = QString("主菜单：%1  子菜单：%2  最大化: %3").arg(type).arg(layoutType).arg(isMax ? "是" : "否");
    ui->label->setText(info);
}
