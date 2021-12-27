#include "widget.h"
#include "ui_widget.h"
#include "qpushbutton.h"
#include "qsignalmapper.h"
#include "qdatetime.h"
#include "qdebug.h"

#define TIMEMS QTime::currentTime().toString("hh:mm:ss zzz")
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->initBtn();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initBtn()
{
    QSignalMapper *signMap = new QSignalMapper(this);
    connect(signMap, SIGNAL(mapped(QString)), this, SLOT(doBtn(QString)));

    int x = 5, y = -25;
    for (int i = 0; i < 1000; ++i) {
        //动态设置坐标
        x += 80;
        if (i % 10 == 0) {
            x = 5;
            y += 30;
        }

        QPushButton *btn = new QPushButton(this);
        btn->setObjectName(QString("btn_%1").arg(i + 1));
        btn->setText(QString("text_%1").arg(i + 1));
        btn->setGeometry(x, y, 75, 25);

        //方法0: 每个按钮关联到一个独立的槽,代码量大不可取放弃
        //方式1: 绑定到一个槽函数
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(doBtn()));
        //方式2: 通过 QSignalMapper 转发信号
        connect(btn, SIGNAL(clicked(bool)), signMap, SLOT(map()));
        signMap->setMapping(btn, btn->objectName());
        //方法3: 用 lambda 表达式
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
        connect(btn, &QPushButton::clicked, [btn] {
            QString name = btn->objectName();
            qDebug() << TIMEMS << "doBtn3" << name;
        });

        connect(btn, &QPushButton::clicked, [=]() {
            QString name = btn->objectName();
            qDebug() << TIMEMS << "doBtn3" << name;
        });
#endif
    }
}

void Widget::doBtn()
{
    QPushButton *btn = (QPushButton *)sender();
    QString name = btn->objectName();
    qDebug() << TIMEMS << "doBtn1" << name;
}

void Widget::doBtn(const QString &name)
{
    qDebug() << TIMEMS << "doBtn2" << name;
}
