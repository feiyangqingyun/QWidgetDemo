#include "widget.h"
#include "ui_widget.h"
#include <QPixmap>
#include <QBitmap>
#include <qmath.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showEvent(QShowEvent *event)
{
    ui->pushButton->setStyleSheet("background-color: red;");
    ui->pushButton2->setStyleSheet("background-color: green;");
    ui->pushButton3->setStyleSheet("background-color: blue;");

    ui->pushButton->setText("button1");
    ui->pushButton2->setText("button2");
    ui->pushButton3->setText("button3");

    QPixmap maskUp(":/bk.png");
    // 获取按钮的大小
    QRect buttonSize = ui->pushButton->geometry();

    // 计算倾斜角度来获取偏移量
    int buttonHeight = buttonSize.height();
    double angleInRadians = qDegreesToRadians(8.0); // 假设倾斜角度为8度
    int horizontalOffset = buttonHeight * qTan(angleInRadians);

    // 将图片缩放到按钮大小
    QPixmap scaledMask = maskUp.scaledToWidth(buttonSize.width());
    ui->pushButton->setMask(scaledMask.mask());
    ui->pushButton2->setMask(scaledMask.mask());
    ui->pushButton3->setMask(scaledMask.mask());
    // 主动布局，不使用固有布局
    ui->pushButton2->move( ui->pushButton->pos().x() + buttonSize.width() - horizontalOffset,ui->pushButton->pos().y());
    ui->pushButton3->move( ui->pushButton2->pos().x() + buttonSize.width() - horizontalOffset,ui->pushButton2->pos().y());
}
