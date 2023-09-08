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

    colorNormal = "black";
    colorAlarm = "red";

    buttonStyle = ButtonStyle_Police;
    buttonColor = ButtonColor_Green;

    isPressed = false;
    lastPoint = QPoint();

    type = "police";
    imgPath = ":/image/devicebutton/devicebutton";
    imgName = QString("%1_green_%2.png").arg(imgPath).arg(type);

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
    //识别鼠标 按下+移动+松开+双击 等事件
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
        //限定鼠标左键
        if (mouseEvent->button() == Qt::LeftButton) {
            lastPoint = mouseEvent->pos();
            isPressed = true;
            emit clicked();
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        //允许拖动并且鼠标按下准备拖动
        if (canMove && isPressed) {
            int dx = mouseEvent->pos().x() - lastPoint.x();
            int dy = mouseEvent->pos().y() - lastPoint.y();
            this->move(this->x() + dx, this->y() + dy);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        isPressed = false;
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    }

    return QWidget::eventFilter(watched, event);
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
        imgName = QString("%1_%2_%3.png").arg(imgPath).arg(colorNormal).arg(type);
    } else {
        imgName = QString("%1_%2_%3.png").arg(imgPath).arg(colorAlarm).arg(type);
    }

    isDark = !isDark;
    this->update();
}

bool DeviceButton::getCanMove() const
{
    return this->canMove;
}

void DeviceButton::setCanMove(bool canMove)
{
    this->canMove = canMove;
}

QString DeviceButton::getText() const
{
    return this->text;
}

void DeviceButton::setText(const QString &text)
{
    if (this->text != text) {
        this->text = text;
        this->update();
    }
}

QString DeviceButton::getColorNormal() const
{
    return this->colorNormal;
}

void DeviceButton::setColorNormal(const QString &colorNormal)
{
    if (this->colorNormal != colorNormal) {
        this->colorNormal = colorNormal;
        this->update();
    }
}

QString DeviceButton::getColorAlarm() const
{
    return this->colorAlarm;
}

void DeviceButton::setColorAlarm(const QString &colorAlarm)
{
    if (this->colorAlarm != colorAlarm) {
        this->colorAlarm = colorAlarm;
        this->update();
    }
}

DeviceButton::ButtonStyle DeviceButton::getButtonStyle() const
{
    return this->buttonStyle;
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

DeviceButton::ButtonColor DeviceButton::getButtonColor() const
{
    return this->buttonColor;
}

void DeviceButton::setButtonColor(const DeviceButton::ButtonColor &buttonColor)
{
    //先停止定时器
    this->buttonColor = buttonColor;
    isDark = false;
    if (timer->isActive()) {
        timer->stop();
    }

    QString color;
    if (buttonColor == ButtonColor_Green) {
        color = "green";
    } else if (buttonColor == ButtonColor_Blue) {
        color = "blue";
    } else if (buttonColor == ButtonColor_Gray) {
        color = "gray";
    } else if (buttonColor == ButtonColor_Black) {
        color = "black";
    } else if (buttonColor == ButtonColor_Purple) {
        color = "purple";
    } else if (buttonColor == ButtonColor_Yellow) {
        color = "yellow";
    } else if (buttonColor == ButtonColor_Red) {
        color = "red";
    } else {
        color = "green";
    }

    //如果和报警颜色一致则主动启动定时器切换报警颜色
    imgName = QString("%1_%2_%3.png").arg(imgPath).arg(color).arg(type);
    if (color == colorAlarm) {
        checkAlarm();
        if (!timer->isActive()) {
            timer->start();
        }
    }

    this->update();
}
