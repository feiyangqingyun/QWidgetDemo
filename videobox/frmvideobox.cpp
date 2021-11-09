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

bool frmVideoBox::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        //双击最大化再次双击还原
        QLabel *widget = (QLabel *) watched;
        if (!max) {
            max = true;
            box->hide_video_all();
            ui->gridLayout->addWidget(widget, 0, 0);
            widget->setVisible(true);
        } else {
            max = false;
            box->show_video_all();
        }

        return true;
    } else if (event->type() == QEvent::MouseButtonPress) {
        //鼠标右键的地方弹出菜单
        if (qApp->mouseButtons() == Qt::RightButton) {
            menu->exec(QCursor::pos());
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmVideoBox::initForm()
{
    max = false;
    //安装事件过滤器
    this->installEventFilter(this);

    //实例化子对象
    QWidgetList widgets;
    for (int i = 0; i < 64; ++i) {
        //这里用QLabel做演示可以改成自己的窗体类比如视频监控窗体
        QLabel *label = new QLabel;
        label->installEventFilter(this);
        label->setFrameShape(QLabel::Box);
        label->setAlignment(Qt::AlignCenter);
        label->setText(QString("通道 %1").arg(i + 1));
        widgets << label;
    }

    //实例化盒子
    box = new VideoBox(this);
    //关联信号槽
    connect(box, SIGNAL(changeVideo(int, QString, bool)), this, SLOT(changeVideo(int, QString, bool)));
    //可以改成 1_4 5_8 1_36 等
    box->setVideoType("1_16");
    box->setLayout(ui->gridLayout);
    box->setWidgets(widgets);
    box->setMenuFlag("排列");
    box->setActionFlag("监控");
    box->show_video_all();

    //实例化菜单
    menu = new QMenu(this);
    //先安排自己的菜单
    //这里关联到一个槽函数处理,也可以关联到不同的槽函数
    menu->addAction("切换全屏模式", this, SLOT(doAction()));
    menu->addAction("启动轮询视频", this, SLOT(doAction()));
    menu->addSeparator();

    //把菜单加到盒子上,并控制布局切换菜单是否可用(默认9个布局切换菜单)
    QList<bool> enable;
    enable << true << true << true << true << true << true << true << true << true;
    box->initMenu(menu, enable);
}

void frmVideoBox::doAction()
{
    //判断是哪个动作触发的
    QAction *action = (QAction *)sender();
    ui->label->setText(QString("触发了菜单： %1").arg(action->text()));
}

void frmVideoBox::changeVideo(int type, const QString &videoType, bool videoMax)
{
    QString info = QString("主菜单：%1  子菜单：%2").arg(type).arg(videoType);
    ui->label->setText(info);
}
