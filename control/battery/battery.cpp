#pragma execution_character_set("utf-8")

#include "battery.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qdebug.h"

Battery::Battery(QWidget *parent) : QWidget(parent)
{
    minValue = 0;
    maxValue = 100;
    value = 0;
    alarmValue = 30;

    animation = true;
    animationStep = 0.5;

    borderWidth = 5;
    borderRadius = 8;
    bgRadius = 5;
    headRadius = 3;

    borderColorStart = QColor(100, 100, 100);
    borderColorEnd = QColor(80, 80, 80);
    alarmColorStart = QColor(250, 118, 113);
    alarmColorEnd = QColor(204, 38, 38);
    normalColorStart = QColor(50, 205, 51);
    normalColorEnd = QColor(60, 179, 133);

    isForward = false;
    currentValue = 0;

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));
}

Battery::~Battery()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void Battery::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制边框
    drawBorder(&painter);
    //绘制背景
    drawBg(&painter);
    //绘制头部
    drawHead(&painter);
}

void Battery::drawBorder(QPainter *painter)
{
    painter->save();

    double headWidth = width() / 15;
    double batteryWidth = width() - headWidth;

    //绘制电池边框
    QPointF topLeft(borderWidth, borderWidth);
    QPointF bottomRight(batteryWidth, height() - borderWidth);
    batteryRect = QRectF(topLeft, bottomRight);

    painter->setPen(QPen(borderColorStart, borderWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(batteryRect, borderRadius, borderRadius);

    painter->restore();
}

void Battery::drawBg(QPainter *painter)
{
    if (value == minValue) {
        return;
    }

    painter->save();

    QLinearGradient batteryGradient(QPointF(0, 0), QPointF(0, height()));
    if (currentValue <= alarmValue) {
        batteryGradient.setColorAt(0.0, alarmColorStart);
        batteryGradient.setColorAt(1.0, alarmColorEnd);
    } else {
        batteryGradient.setColorAt(0.0, normalColorStart);
        batteryGradient.setColorAt(1.0, normalColorEnd);
    }

    int margin = qMin(width(), height()) / 20;
    double unit = (batteryRect.width() - (margin * 2)) / (maxValue - minValue);
    double width = currentValue * unit;
    QPointF topLeft(batteryRect.topLeft().x() + margin, batteryRect.topLeft().y() + margin);
    QPointF bottomRight(width + margin + borderWidth, batteryRect.bottomRight().y() - margin);
    QRectF rect(topLeft, bottomRight);

    painter->setPen(Qt::NoPen);
    painter->setBrush(batteryGradient);
    painter->drawRoundedRect(rect, bgRadius, bgRadius);

    painter->restore();
}

void Battery::drawHead(QPainter *painter)
{
    painter->save();

    QPointF headRectTopLeft(batteryRect.topRight().x(), height() / 3);
    QPointF headRectBottomRight(width(), height() - height() / 3);
    QRectF headRect(headRectTopLeft, headRectBottomRight);

    QLinearGradient headRectGradient(headRect.topLeft(), headRect.bottomLeft());
    headRectGradient.setColorAt(0.0, borderColorStart);
    headRectGradient.setColorAt(1.0, borderColorEnd);

    painter->setPen(Qt::NoPen);
    painter->setBrush(headRectGradient);
    painter->drawRoundedRect(headRect, headRadius, headRadius);

    painter->restore();
}

void Battery::updateValue()
{
    if (isForward) {
        currentValue -= animationStep;
        if (currentValue <= value) {
            currentValue = value;
            timer->stop();
        }
    } else {
        currentValue += animationStep;
        if (currentValue >= value) {
            currentValue = value;
            timer->stop();
        }
    }

    this->update();
}

QSize Battery::sizeHint() const
{
    return QSize(150, 80);
}

QSize Battery::minimumSizeHint() const
{
    return QSize(30, 10);
}

void Battery::setRange(double minValue, double maxValue)
{
    //如果最小值大于或者等于最大值则不设置
    if (minValue >= maxValue) {
        return;
    }

    this->minValue = minValue;
    this->maxValue = maxValue;

    //如果目标值不在范围值内,则重新设置目标值
    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue) {
        setValue(minValue);
    } else if (value > maxValue) {
        setValue(maxValue);
    }

    this->update();
}

void Battery::setRange(int minValue, int maxValue)
{
    setRange((double)minValue, (double)maxValue);
}

double Battery::getMinValue() const
{
    return this->minValue;
}

void Battery::setMinValue(double minValue)
{
    setRange(minValue, maxValue);
}

double Battery::getMaxValue() const
{
    return this->maxValue;
}

void Battery::setMaxValue(double maxValue)
{
    setRange(minValue, maxValue);
}

double Battery::getValue() const
{
    return this->value;
}

void Battery::setValue(double value)
{
    //值和当前值一致则无需处理
    if (value == this->value) {
        return;
    }

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue) {
        value = minValue;
    } else if (value > maxValue) {
        value = maxValue;
    }

    if (value > currentValue) {
        isForward = false;
    } else if (value < currentValue) {
        isForward = true;
    } else {
        this->value = value;
        this->update();
        return;
    }

    this->value = value;
    emit valueChanged(value);
    if (animation) {
        timer->stop();
        timer->start();
    } else {
        this->currentValue = value;
        this->update();
    }
}

void Battery::setValue(int value)
{
    setValue((double)value);
}

double Battery::getAlarmValue() const
{
    return this->alarmValue;
}

void Battery::setAlarmValue(double alarmValue)
{
    if (this->alarmValue != alarmValue) {
        this->alarmValue = alarmValue;
        this->update();
    }
}

void Battery::setAlarmValue(int alarmValue)
{
    setAlarmValue((double)alarmValue);
}


bool Battery::getAnimation() const
{
    return this->animation;
}

void Battery::setAnimation(bool animation)
{
    if (this->animation != animation) {
        this->animation = animation;
        this->update();
    }
}

double Battery::getAnimationStep() const
{
    return this->animationStep;
}

void Battery::setAnimationStep(double animationStep)
{
    if (this->animationStep != animationStep) {
        this->animationStep = animationStep;
        this->update();
    }
}

int Battery::getBorderWidth() const
{
    return this->borderWidth;
}

void Battery::setBorderWidth(int borderWidth)
{
    if (this->borderWidth != borderWidth) {
        this->borderWidth = borderWidth;
        this->update();
    }
}

int Battery::getBorderRadius() const
{
    return this->borderRadius;
}

void Battery::setBorderRadius(int borderRadius)
{
    if (this->borderRadius != borderRadius) {
        this->borderRadius = borderRadius;
        this->update();
    }
}

int Battery::getBgRadius() const
{
    return this->bgRadius;
}

void Battery::setBgRadius(int bgRadius)
{
    if (this->bgRadius != bgRadius) {
        this->bgRadius = bgRadius;
        this->update();
    }
}

int Battery::getHeadRadius() const
{
    return this->headRadius;
}

void Battery::setHeadRadius(int headRadius)
{
    if (this->headRadius != headRadius) {
        this->headRadius = headRadius;
        this->update();
    }
}

QColor Battery::getBorderColorStart() const
{
    return this->borderColorStart;
}

void Battery::setBorderColorStart(const QColor &borderColorStart)
{
    if (this->borderColorStart != borderColorStart) {
        this->borderColorStart = borderColorStart;
        this->update();
    }
}

QColor Battery::getBorderColorEnd() const
{
    return this->borderColorEnd;
}

void Battery::setBorderColorEnd(const QColor &borderColorEnd)
{
    if (this->borderColorEnd != borderColorEnd) {
        this->borderColorEnd = borderColorEnd;
        this->update();
    }
}

QColor Battery::getAlarmColorStart() const
{
    return this->alarmColorStart;
}

void Battery::setAlarmColorStart(const QColor &alarmColorStart)
{
    if (this->alarmColorStart != alarmColorStart) {
        this->alarmColorStart = alarmColorStart;
        this->update();
    }
}

QColor Battery::getAlarmColorEnd() const
{
    return this->alarmColorEnd;
}

void Battery::setAlarmColorEnd(const QColor &alarmColorEnd)
{
    if (this->alarmColorEnd != alarmColorEnd) {
        this->alarmColorEnd = alarmColorEnd;
        this->update();
    }
}

QColor Battery::getNormalColorStart() const
{
    return this->normalColorStart;
}

void Battery::setNormalColorStart(const QColor &normalColorStart)
{
    if (this->normalColorStart != normalColorStart) {
        this->normalColorStart = normalColorStart;
        this->update();
    }
}

QColor Battery::getNormalColorEnd() const
{
    return this->normalColorEnd;
}

void Battery::setNormalColorEnd(const QColor &normalColorEnd)
{
    if (this->normalColorEnd != normalColorEnd) {
        this->normalColorEnd = normalColorEnd;
        this->update();
    }
}
