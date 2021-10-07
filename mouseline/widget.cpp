#include "widget.h"
#include "ui_widget.h"
#include "qpainter.h"
#include "qevent.h"
#include "qdebug.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    update();
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    //这里是鼠标按下的坐标,自己存到数据库
    lastPos = event->pos();
    update();
    qDebug() << lastPos;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::red);
    painter.setPen(pen);

    //绘制横向线
    painter.drawLine(0, lastPos.y(), width(), lastPos.y());
    //绘制纵向线
    painter.drawLine(lastPos.x(), 0, lastPos.x(), height());
}
