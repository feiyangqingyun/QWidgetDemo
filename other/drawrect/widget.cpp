#include "widget.h"
#include "ui_widget.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qdebug.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    interval = 20;
    count = 500;
    this->setWindowTitle(QString("随机大量矩形  帧率: %1  数量: %2 (QQ: 517216493 WX: feiyangqingyun)").arg(1000 / interval).arg(count));

    //定时器测试速度
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(interval);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();
    QPainter painter(this);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter.setPen(pen);

    for (int i = 0; i < count; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        painter.drawRect(x, y, 30, 30);
    }
}
