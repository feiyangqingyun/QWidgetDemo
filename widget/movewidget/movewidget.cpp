#include "movewidget.h"
#include "qevent.h"
#include "qdebug.h"

MoveWidget::MoveWidget(QObject *parent) : QObject(parent)
{
    lastPoint = QPoint(0, 0);
    pressed = false;
    leftButton = true;
    inControl = true;
    widget = 0;
}

bool MoveWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (widget && watched == widget) {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            //如果限定了只能鼠标左键拖动则判断当前是否是鼠标左键
            if (leftButton && mouseEvent->button() != Qt::LeftButton) {
                return false;
            }

            //判断控件的区域是否包含了当前鼠标的坐标
            if (widget->rect().contains(mouseEvent->pos())) {
                lastPoint = mouseEvent->pos();
                pressed = true;
            }
        } else if (mouseEvent->type() == QEvent::MouseMove && pressed) {
            //计算坐标偏移值,调用move函数移动过去
            int offsetX = mouseEvent->pos().x() - lastPoint.x();
            int offsetY = mouseEvent->pos().y() - lastPoint.y();
            int x = widget->x() + offsetX;
            int y = widget->y() + offsetY;
            if (inControl) {
                //可以自行调整限定在容器中的范围,这里默认保留20个像素在里面
                int offset = 20;
                bool xyOut = (x + widget->width() < offset || y + widget->height() < offset);
                bool whOut = false;
                QWidget *w = (QWidget *)widget->parent();
                if (w) {
                    whOut = (w->width() - x < offset || w->height() - y < offset);
                }
                if (xyOut || whOut) {
                    return false;
                }
            }

            widget->move(x, y);
        } else if (mouseEvent->type() == QEvent::MouseButtonRelease && pressed) {
            pressed = false;
        }
    }

    return QObject::eventFilter(watched, event);
}

void MoveWidget::setLeftButton(bool leftButton)
{
    this->leftButton = leftButton;
}

void MoveWidget::setInControl(bool inControl)
{
    this->inControl = inControl;
}

void MoveWidget::setWidget(QWidget *widget)
{
    if (this->widget == 0) {
        this->widget = widget;
        this->widget->installEventFilter(this);
    }
}
