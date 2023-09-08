#pragma execution_character_set("utf-8")

#include "navbutton.h"
#include "qpainter.h"
#include "qdebug.h"

NavButton::NavButton(QWidget *parent) : QPushButton(parent)
{
    paddingLeft = 20;
    paddingRight = 5;
    paddingTop = 5;
    paddingBottom = 5;
    textAlign = TextAlign_Left;

    showTriangle = false;
    triangleLen = 5;
    trianglePosition = TrianglePosition_Right;
    triangleColor = QColor(255, 255, 255);

    showIcon = false;
    iconSpace = 10;
    iconSize = QSize(16, 16);
    iconNormal = QPixmap();
    iconHover = QPixmap();
    iconCheck = QPixmap();

    showLine = true;
    lineSpace = 0;
    lineWidth = 5;
    linePosition = LinePosition_Left;
    lineColor = QColor(0, 187, 158);

    normalBgColor = QColor(230, 230, 230);
    hoverBgColor = QColor(130, 130, 130);
    checkBgColor = QColor(80, 80, 80);
    normalTextColor = QColor(100, 100, 100);
    hoverTextColor = QColor(255, 255, 255);
    checkTextColor = QColor(255, 255, 255);

    normalBgBrush = Qt::NoBrush;
    hoverBgBrush = Qt::NoBrush;
    checkBgBrush = Qt::NoBrush;

    hover = false;
    setCheckable(true);
    setText("导航按钮");
}

void NavButton::enterEvent(QEvent *)
{
    hover = true;
    this->update();
}

void NavButton::leaveEvent(QEvent *)
{
    hover = false;
    this->update();
}

void NavButton::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景
    drawBg(&painter);
    //绘制文字
    drawText(&painter);
    //绘制图标
    drawIcon(&painter);
    //绘制边框线条
    drawLine(&painter);
    //绘制倒三角
    drawTriangle(&painter);
}

void NavButton::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    int width = this->width();
    int height = this->height();

    QRect bgRect;
    if (linePosition == LinePosition_Left) {
        bgRect = QRect(lineSpace, 0, width - lineSpace, height);
    } else if (linePosition == LinePosition_Right) {
        bgRect = QRect(0, 0, width - lineSpace, height);
    } else if (linePosition == LinePosition_Top) {
        bgRect = QRect(0, lineSpace, width, height - lineSpace);
    } else if (linePosition == LinePosition_Bottom) {
        bgRect = QRect(0, 0, width, height - lineSpace);
    }

    //如果画刷存在则取画刷
    QBrush bgBrush;
    if (isChecked()) {
        bgBrush = checkBgBrush;
    } else if (hover) {
        bgBrush = hoverBgBrush;
    } else {
        bgBrush = normalBgBrush;
    }

    if (bgBrush != Qt::NoBrush) {
        painter->setBrush(bgBrush);
    } else {
        //根据当前状态选择对应颜色
        QColor bgColor;
        if (isChecked()) {
            bgColor = checkBgColor;
        } else if (hover) {
            bgColor = hoverBgColor;
        } else {
            bgColor = normalBgColor;
        }

        painter->setBrush(bgColor);
    }

    painter->drawRect(bgRect);

    painter->restore();
}

void NavButton::drawText(QPainter *painter)
{
    painter->save();
    painter->setBrush(Qt::NoBrush);

    //根据当前状态选择对应颜色
    QColor textColor;
    if (isChecked()) {
        textColor = checkTextColor;
    } else if (hover) {
        textColor = hoverTextColor;
    } else {
        textColor = normalTextColor;
    }

    QRect textRect = QRect(paddingLeft, paddingTop, width() - paddingLeft - paddingRight, height() - paddingTop - paddingBottom);
    painter->setPen(textColor);
    painter->drawText(textRect, textAlign | Qt::AlignVCenter, text());

    painter->restore();
}

void NavButton::drawIcon(QPainter *painter)
{
    if (!showIcon) {
        return;
    }

    painter->save();

    QPixmap pix;
    if (isChecked()) {
        pix = iconCheck;
    } else if (hover) {
        pix = iconHover;
    } else {
        pix = iconNormal;
    }

    if (!pix.isNull()) {
        //等比例平滑缩放图标
        pix = pix.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(iconSpace, (height() - iconSize.height()) / 2, pix);
    }

    painter->restore();
}

void NavButton::drawLine(QPainter *painter)
{
    if (!showLine) {
        return;
    }

    if (!isChecked()) {
        return;
    }

    painter->save();

    QPen pen;
    pen.setWidth(lineWidth);
    pen.setColor(lineColor);
    painter->setPen(pen);

    //根据线条位置设置线条坐标
    QPoint pointStart, pointEnd;
    if (linePosition == LinePosition_Left) {
        pointStart = QPoint(0, 0);
        pointEnd = QPoint(0, height());
    } else if (linePosition == LinePosition_Right) {
        pointStart = QPoint(width(), 0);
        pointEnd = QPoint(width(), height());
    } else if (linePosition == LinePosition_Top) {
        pointStart = QPoint(0, 0);
        pointEnd = QPoint(width(), 0);
    } else if (linePosition == LinePosition_Bottom) {
        pointStart = QPoint(0, height());
        pointEnd = QPoint(width(), height());
    }

    painter->drawLine(pointStart, pointEnd);

    painter->restore();
}

void NavButton::drawTriangle(QPainter *painter)
{
    if (!showTriangle) {
        return;
    }

    //选中或者悬停显示
    if (!hover && !isChecked()) {
        return;
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(triangleColor);

    //绘制在右侧中间,根据设定的倒三角的边长设定三个点位置
    int width = this->width();
    int height = this->height();
    int midWidth = width / 2;
    int midHeight = height / 2;

    QPolygon pts;
    if (trianglePosition == TrianglePosition_Left) {
        pts.setPoints(3, triangleLen, midHeight, 0, midHeight - triangleLen, 0, midHeight + triangleLen);
    } else if (trianglePosition == TrianglePosition_Right) {
        pts.setPoints(3, width - triangleLen, midHeight, width, midHeight - triangleLen, width, midHeight + triangleLen);
    } else if (trianglePosition == TrianglePosition_Top) {
        pts.setPoints(3, midWidth, triangleLen, midWidth - triangleLen, 0, midWidth + triangleLen, 0);
    } else if (trianglePosition == TrianglePosition_Bottom) {
        pts.setPoints(3, midWidth, height - triangleLen, midWidth - triangleLen, height, midWidth + triangleLen, height);
    }

    painter->drawPolygon(pts);

    painter->restore();
}

QSize NavButton::sizeHint() const
{
    return QSize(100, 30);
}

QSize NavButton::minimumSizeHint() const
{
    return QSize(20, 10);
}

int NavButton::getPaddingLeft() const
{
    return this->paddingLeft;
}

void NavButton::setPaddingLeft(int paddingLeft)
{
    if (this->paddingLeft != paddingLeft) {
        this->paddingLeft = paddingLeft;
        this->update();
    }
}

int NavButton::getPaddingRight() const
{
    return this->paddingRight;
}

void NavButton::setPaddingRight(int paddingRight)
{
    if (this->paddingRight != paddingRight) {
        this->paddingRight = paddingRight;
        this->update();
    }
}

int NavButton::getPaddingTop() const
{
    return this->paddingTop;
}

void NavButton::setPaddingTop(int paddingTop)
{
    if (this->paddingTop != paddingTop) {
        this->paddingTop = paddingTop;
        this->update();
    }
}

int NavButton::getPaddingBottom() const
{
    return this->paddingBottom;
}

void NavButton::setPaddingBottom(int paddingBottom)
{
    if (this->paddingBottom != paddingBottom) {
        this->paddingBottom = paddingBottom;
        this->update();
    }
}

void NavButton::setPadding(int padding)
{
    setPadding(padding, padding, padding, padding);
}

void NavButton::setPadding(int paddingLeft, int paddingRight, int paddingTop, int paddingBottom)
{
    this->paddingLeft = paddingLeft;
    this->paddingRight = paddingRight;
    this->paddingTop = paddingTop;
    this->paddingBottom = paddingBottom;
    this->update();
}

NavButton::TextAlign NavButton::getTextAlign() const
{
    return this->textAlign;
}

void NavButton::setTextAlign(const NavButton::TextAlign &textAlign)
{
    if (this->textAlign != textAlign) {
        this->textAlign = textAlign;
        this->update();
    }
}

bool NavButton::getShowTriangle() const
{
    return this->showTriangle;
}

void NavButton::setShowTriangle(bool showTriangle)
{
    if (this->showTriangle != showTriangle) {
        this->showTriangle = showTriangle;
        this->update();
    }
}

int NavButton::getTriangleLen() const
{
    return this->triangleLen;
}

void NavButton::setTriangleLen(int triangleLen)
{
    if (this->triangleLen != triangleLen) {
        this->triangleLen = triangleLen;
        this->update();
    }
}

NavButton::TrianglePosition NavButton::getTrianglePosition() const
{
    return this->trianglePosition;
}

void NavButton::setTrianglePosition(const NavButton::TrianglePosition &trianglePosition)
{
    if (this->trianglePosition != trianglePosition) {
        this->trianglePosition = trianglePosition;
        this->update();
    }
}

QColor NavButton::getTriangleColor() const
{
    return this->triangleColor;
}

void NavButton::setTriangleColor(const QColor &triangleColor)
{
    if (this->triangleColor != triangleColor) {
        this->triangleColor = triangleColor;
        this->update();
    }
}

bool NavButton::getShowIcon() const
{
    return this->showIcon;
}

void NavButton::setShowIcon(bool showIcon)
{
    if (this->showIcon != showIcon) {
        this->showIcon = showIcon;
        this->update();
    }
}

int NavButton::getIconSpace() const
{
    return this->iconSpace;
}

void NavButton::setIconSpace(int iconSpace)
{
    if (this->iconSpace != iconSpace) {
        this->iconSpace = iconSpace;
        this->update();
    }
}

QSize NavButton::getIconSize() const
{
    return this->iconSize;
}

void NavButton::setIconSize(const QSize &iconSize)
{
    if (this->iconSize != iconSize) {
        this->iconSize = iconSize;
        this->update();
    }
}

QPixmap NavButton::getIconNormal() const
{
    return this->iconNormal;
}

void NavButton::setIconNormal(const QPixmap &iconNormal)
{
    this->iconNormal = iconNormal;
    this->update();
}

QPixmap NavButton::getIconHover() const
{
    return this->iconHover;
}

void NavButton::setIconHover(const QPixmap &iconHover)
{
    this->iconHover = iconHover;
    this->update();
}

QPixmap NavButton::getIconCheck() const
{
    return this->iconCheck;
}

void NavButton::setIconCheck(const QPixmap &iconCheck)
{
    this->iconCheck = iconCheck;
    this->update();
}

bool NavButton::getShowLine() const
{
    return this->showLine;
}

void NavButton::setShowLine(bool showLine)
{
    if (this->showLine != showLine) {
        this->showLine = showLine;
        this->update();
    }
}

int NavButton::getLineSpace() const
{
    return this->lineSpace;
}

void NavButton::setLineSpace(int lineSpace)
{
    if (this->lineSpace != lineSpace) {
        this->lineSpace = lineSpace;
        this->update();
    }
}

int NavButton::getLineWidth() const
{
    return this->lineWidth;
}

void NavButton::setLineWidth(int lineWidth)
{
    if (this->lineWidth != lineWidth) {
        this->lineWidth = lineWidth;
        this->update();
    }
}

NavButton::LinePosition NavButton::getLinePosition() const
{
    return this->linePosition;
}

void NavButton::setLinePosition(const NavButton::LinePosition &linePosition)
{
    if (this->linePosition != linePosition) {
        this->linePosition = linePosition;
        this->update();
    }
}

QColor NavButton::getLineColor() const
{
    return this->lineColor;
}

void NavButton::setLineColor(const QColor &lineColor)
{
    if (this->lineColor != lineColor) {
        this->lineColor = lineColor;
        this->update();
    }
}

QColor NavButton::getNormalBgColor() const
{
    return this->normalBgColor;
}

void NavButton::setNormalBgColor(const QColor &normalBgColor)
{
    if (this->normalBgColor != normalBgColor) {
        this->normalBgColor = normalBgColor;
        this->update();
    }
}

QColor NavButton::getHoverBgColor() const
{
    return this->hoverBgColor;
}

void NavButton::setHoverBgColor(const QColor &hoverBgColor)
{
    if (this->hoverBgColor != hoverBgColor) {
        this->hoverBgColor = hoverBgColor;
        this->update();
    }
}

QColor NavButton::getCheckBgColor() const
{
    return this->checkBgColor;
}

void NavButton::setCheckBgColor(const QColor &checkBgColor)
{
    if (this->checkBgColor != checkBgColor) {
        this->checkBgColor = checkBgColor;
        this->update();
    }
}

QColor NavButton::getNormalTextColor() const
{
    return this->normalTextColor;
}

void NavButton::setNormalTextColor(const QColor &normalTextColor)
{
    if (this->normalTextColor != normalTextColor) {
        this->normalTextColor = normalTextColor;
        this->update();
    }
}


QColor NavButton::getHoverTextColor() const
{
    return this->hoverTextColor;
}

void NavButton::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        this->update();
    }
}

QColor NavButton::getCheckTextColor() const
{
    return this->checkTextColor;
}

void NavButton::setCheckTextColor(const QColor &checkTextColor)
{
    if (this->checkTextColor != checkTextColor) {
        this->checkTextColor = checkTextColor;
        this->update();
    }
}

void NavButton::setNormalBgBrush(const QBrush &normalBgBrush)
{
    if (this->normalBgBrush != normalBgBrush) {
        this->normalBgBrush = normalBgBrush;
        this->update();
    }
}

void NavButton::setHoverBgBrush(const QBrush &hoverBgBrush)
{
    if (this->hoverBgBrush != hoverBgBrush) {
        this->hoverBgBrush = hoverBgBrush;
        this->update();
    }
}

void NavButton::setCheckBgBrush(const QBrush &checkBgBrush)
{
    if (this->checkBgBrush != checkBgBrush) {
        this->checkBgBrush = checkBgBrush;
        this->update();
    }
}
