#pragma execution_character_set("utf-8")

#include "devicebutton.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

DeviceButton::DeviceButton(QWidget *parent) : QWidget(parent)
{
    canMove = false;
    text = "1";
    buttonStyle = ButtonStyle_Police;
    buttonColor = ButtonColor_Green;

    type = "police";
    imgName = QString(":/image/devicebutton/devicebutton_green_%1.png").arg(type);
    isDark = false;

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));

    this->installEventFilter(this);
}

DeviceButton::~DeviceButton()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void DeviceButton::paintEvent(QPaintEvent *)
{
    double width = this->width();
    double height = this->height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //绘制背景图
    QImage img(imgName);
    if (!img.isNull()) {
        img = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawImage(0, 0, img);
    }

    //计算字体
    QFont font;
    font.setPixelSize(height * 0.37);
    font.setBold(true);

    //自动计算文字绘制区域,绘制防区号
    QRectF rect = this->rect();
    if (buttonStyle == ButtonStyle_Police) {
        double y = (30 * height / 60);
        rect = QRectF(0, y, width, height - y);
    } else if (buttonStyle == ButtonStyle_Bubble) {
        double y = (8 * height / 60);
        rect = QRectF(0, 0, width, height - y);
    } else if (buttonStyle == ButtonStyle_Bubble2) {
        double y = (13 * height / 60);
        rect = QRectF(0, 0, width, height - y);
        font.setPixelSize(width * 0.33);
    } else if (buttonStyle == ButtonStyle_Msg) {
        double y = (17 * height / 60);
        rect = QRectF(0, 0, width, height - y);
    } else if (buttonStyle == ButtonStyle_Msg2) {
        double y = (17 * height / 60);
        rect = QRectF(0, 0, width, height - y);
    }

    //绘制文字标识
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(rect, Qt::AlignCenter, text);
}

bool DeviceButton::eventFilter(QObject *watched, QEvent *event)
{
    if (canMove) {
        static QPoint lastPoint;
        static bool isPressed = false;

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if (this->rect().contains(e->pos()) && (e->button() == Qt::LeftButton)) {
                lastPoint = e->pos();
                isPressed = true;
            }
        } else if (event->type() == QEvent::MouseMove && isPressed) {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            int dx = e->pos().x() - lastPoint.x();
            int dy = e->pos().y() - lastPoint.y();
            this->move(this->x() + dx, this->y() + dy);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease && isPressed) {
            isPressed = false;
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {
        emit clicked();
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    }

    return QWidget::eventFilter(watched, event);
}

bool DeviceButton::getCanMove() const
{
    return this->canMove;
}

QString DeviceButton::getText() const
{
    return this->text;
}

DeviceButton::ButtonStyle DeviceButton::getButtonStyle() const
{
    return this->buttonStyle;
}

DeviceButton::ButtonColor DeviceButton::getButtonColor() const
{
    return this->buttonColor;
}

QSize DeviceButton::sizeHint() const
{
    return QSize(50, 50);
}

QSize DeviceButton::minimumSizeHint() const
{
    return QSize(10, 10);
}

void DeviceButton::checkAlarm()
{
    if (isDark) {
        imgName = QString(":/image/devicebutton/devicebutton_black_%1.png").arg(type);
    } else {
        imgName = QString(":/image/devicebutton/devicebutton_red_%1.png").arg(type);
    }

    isDark = !isDark;
    this->update();
}

void DeviceButton::setCanMove(bool canMove)
{
    this->canMove = canMove;
}

void DeviceButton::setText(const QString &text)
{
    if (this->text != text) {
        this->text = text;
        this->update();
    }
}

void DeviceButton::setButtonStyle(const DeviceButton::ButtonStyle &buttonStyle)
{
    this->buttonStyle = buttonStyle;
    if (buttonStyle == ButtonStyle_Circle) {
        type = "circle";
    } else if (buttonStyle == ButtonStyle_Police) {
        type = "police";
    } else if (buttonStyle == ButtonStyle_Bubble) {
        type = "bubble";
    } else if (buttonStyle == ButtonStyle_Bubble2) {
        type = "bubble2";
    } else if (buttonStyle == ButtonStyle_Msg) {
        type = "msg";
    } else if (buttonStyle == ButtonStyle_Msg2) {
        type = "msg2";
    } else {
        type = "circle";
    }

    setButtonColor(buttonColor);
}

void DeviceButton::setButtonColor(const DeviceButton::ButtonColor &buttonColor)
{
    this->buttonColor = buttonColor;
    isDark = false;
    if (timer->isActive()) {
        timer->stop();
    }

    if (buttonColor == ButtonColor_Green) {
        imgName = QString(":/image/devicebutton/devicebutton_green_%1.png").arg(type);
    } else if (buttonColor == ButtonColor_Blue) {
        imgName = QString(":/image/devicebutton/devicebutton_blue_%1.png").arg(type);
    } else if (buttonColor == ButtonColor_Gray) {
        imgName = QString(":/image/devicebutton/devicebutton_gray_%1.png").arg(type);
    } else if (buttonColor == ButtonColor_Black) {
        imgName = QString(":/image/devicebutton/devicebutton_black_%1.png").arg(type);
    } else if (buttonColor == ButtonColor_Red) {
        checkAlarm();
        if (!timer->isActive()) {
            timer->start();
        }
    }

    this->update();
}
