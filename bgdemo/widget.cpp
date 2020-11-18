#include "widget.h"
#include "ui_widget.h"
#include "qevent.h"
#include "qdebug.h"
#include <QTimer>
#include<QWheelEvent>
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    ui->widget->installEventFilter(this);
    ui->widget->setStyleSheet(QString("background-image:url(:/image/%1.png);").arg(1));
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *watched, QEvent *evt)
{
    static int index = 1;
    static QPoint mousePoint;
    static bool mousePressed = false;
    static bool is_smooth_scrolling = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - this->pos();

            if (index == 5) {
                index = 1;
            } else {
                index++;
            }

            ui->widget->setStyleSheet(QString("background-image:url(:/image/%1.png);").arg(index));

            return true;
        } else {
            exit(0);
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() && Qt::LeftButton)) {
            this->move(event->globalPos() - mousePoint);
            return true;
        }
    }

    if (event->type()  == QEvent::Wheel) {
        QWheelEvent *event = static_cast<QWheelEvent *>(evt);
        if(is_smooth_scrolling == false) {
          if (event->delta() > 0) {
              is_smooth_scrolling = true;
              if (index == 5) {
                 index = 1;
              } else {
                  index++;
              }
          } else if (event->delta() < 0) {
              is_smooth_scrolling = true;
              if (index != 1) {
                 index--;
              } else {
                  index =5;
              }
          }
          ui->widget->setStyleSheet(QString("background-image:url(:/image/%1.png);").arg(index));
          QTimer::singleShot(400, [&]() { is_smooth_scrolling = false; });
          return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
