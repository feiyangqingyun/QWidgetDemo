#pragma execution_character_set("utf-8")

#include "buttondefence.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

ButtonDefence::ButtonDefence(QWidget *parent) : QWidget(parent)
{
    canMove = false;
    text = "1";
    buttonStyle = ButtonStyle_Police;
    buttonStatus = ButtonStatus_Arming;

    type = "police";
    imgName = QString(":/image/buttondefence/btn_defence_disarming_%1.png").arg(type);
    isDark = false;

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));

    this->installEventFilter(this);
}

ButtonDefence::~ButtonDefence()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void ButtonDefence::paintEvent(QPaintEvent *)
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

bool ButtonDefence::eventFilter(QObject *watched, QEvent *event)
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

bool ButtonDefence::getCanMove() const
{
    return this->canMove;
}

QString ButtonDefence::getText() const
{
    return this->text;
}

ButtonDefence::ButtonStyle ButtonDefence::getButtonStyle() const
{
    return this->buttonStyle;
}

ButtonDefence::ButtonStatus ButtonDefence::getButtonStatus() const
{
    return this->buttonStatus;
}

QSize ButtonDefence::sizeHint() const
{
    return QSize(50, 50);
}

QSize ButtonDefence::minimumSizeHint() const
{
    return QSize(10, 10);
}

void ButtonDefence::checkAlarm()
{
    if (isDark) {
        imgName = QString(":/image/buttondefence/btn_defence_error_%1.png").arg(type);
    } else {
        imgName = QString(":/image/buttondefence/btn_defence_alarm_%1.png").arg(type);
    }

    isDark = !isDark;
    this->update();
}

void ButtonDefence::setCanMove(bool canMove)
{
    this->canMove = canMove;
}

void ButtonDefence::setText(const QString &text)
{
    if (this->text != text) {
        this->text = text;
        this->update();
    }
}

void ButtonDefence::setButtonStyle(const ButtonDefence::ButtonStyle &buttonStyle)
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

    setButtonStatus(buttonStatus);
}

void ButtonDefence::setButtonStatus(const ButtonDefence::ButtonStatus &buttonStatus)
{
    this->buttonStatus = buttonStatus;
    isDark = false;
    if (timer->isActive()) {
        timer->stop();
    }

    if (buttonStatus == ButtonStatus_Arming) {
        imgName = QString(":/image/buttondefence/btn_defence_arming_%1.png").arg(type);
    } else if (buttonStatus == ButtonStatus_Disarming) {
        imgName = QString(":/image/buttondefence/btn_defence_disarming_%1.png").arg(type);
    } else if (buttonStatus == ButtonStatus_Bypass) {
        imgName = QString(":/image/buttondefence/btn_defence_bypass_%1.png").arg(type);
    } else if (buttonStatus == ButtonStatus_Error) {
        imgName = QString(":/image/buttondefence/btn_defence_error_%1.png").arg(type);
    } else if (buttonStatus == ButtonStatus_Alarm) {
        checkAlarm();
        if (!timer->isActive()) {
            timer->start();
        }
    }

    this->update();
}
