#pragma execution_character_set("utf-8")

#include "lunarcalendaritem.h"
#include "qpainter.h"
#include "qevent.h"
#include "qdatetime.h"
#include "qdebug.h"

LunarCalendarItem::LunarCalendarItem(QWidget *parent) : QWidget(parent)
{
    hover = false;
    pressed = false;
    listDayName << "*" << "初一" << "初二" << "初三" << "初四" << "初五" << "初六" << "初七" << "初八" << "初九" << "初十"
                << "十一" << "十二" << "十三" << "十四" << "十五" << "十六" << "十七" << "十八" << "十九" << "二十"
                << "廿一" << "廿二" << "廿三" << "廿四" << "廿五" << "廿六" << "廿七" << "廿八" << "廿九" << "三十";

    select = false;
    showLunar = true;
    bgImage = ":/image/bg_calendar.png";
    selectType = SelectType_Rect;

    date = QDate::currentDate();
    lunar = "初一";
    dayType = DayType_MonthCurrent;

    borderColor = QColor(180, 180, 180);
    weekColor = QColor(255, 0, 0);
    superColor = QColor(255, 129, 6);
    lunarColor = QColor(55, 156, 238);

    currentTextColor = QColor(0, 0, 0);
    otherTextColor = QColor(200, 200, 200);
    selectTextColor = QColor(255, 255, 255);
    hoverTextColor = QColor(250, 250, 250);

    currentLunarColor = QColor(150, 150, 150);
    otherLunarColor = QColor(200, 200, 200);
    selectLunarColor = QColor(255, 255, 255);
    hoverLunarColor = QColor(250, 250, 250);

    currentBgColor = QColor(255, 255, 255);
    otherBgColor = QColor(240, 240, 240);
    selectBgColor = QColor(208, 47, 18);
    hoverBgColor = QColor(204, 183, 180);
}

void LunarCalendarItem::enterEvent(QEvent *)
{
    hover = true;
    this->update();
}

void LunarCalendarItem::leaveEvent(QEvent *)
{
    hover = false;
    this->update();
}

void LunarCalendarItem::mousePressEvent(QMouseEvent *)
{
    pressed = true;
    this->update();

    emit clicked(date, dayType);
}

void LunarCalendarItem::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
    this->update();
}

void LunarCalendarItem::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景和边框
    drawBg(&painter);

    //优先绘制选中状态,其次绘制悬停状态
    if (select) {
        drawBgCurrent(&painter, selectBgColor);
    } else if (hover) {
        drawBgCurrent(&painter, hoverBgColor);
    }

    //绘制日期
    drawDay(&painter);

    //绘制农历信息
    drawLunar(&painter);
}

void LunarCalendarItem::drawBg(QPainter *painter)
{
    painter->save();

    //根据当前类型选择对应的颜色
    QColor bgColor = currentBgColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        bgColor = otherBgColor;
    }

    painter->setPen(borderColor);
    painter->setBrush(bgColor);
    painter->drawRect(rect());

    painter->restore();
}

void LunarCalendarItem::drawBgCurrent(QPainter *painter, const QColor &color)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    painter->save();

    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    //根据设定绘制背景样式
    if (selectType == SelectType_Rect) {
        painter->drawRect(rect());
    } else if (selectType == SelectType_Circle) {
        int radius = side / 2 - 3;
        painter->drawEllipse(QPointF(width / 2, height / 2), radius, radius);
    } else if (selectType == SelectType_Triangle) {
        int radius = side / 3;
        QPolygon pts;
        pts.setPoints(3, 1, 1, radius, 1, 1, radius);
        painter->drawRect(rect());
        painter->setBrush(superColor);
        painter->drawConvexPolygon(pts);
    } else if (selectType == SelectType_Image) {
        //等比例缩放居中绘制
        QImage img(bgImage);
        if (!img.isNull()) {
            img = img.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = (width - img.width()) / 2;
            int y = (height - img.height()) / 2;
            painter->drawImage(x, y, img);
        }
    }

    painter->restore();
}

void LunarCalendarItem::drawDay(QPainter *painter)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    painter->save();

    //根据当前类型选择对应的颜色
    QColor color = currentTextColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        color = otherTextColor;
    } else if (dayType == DayType_WeekEnd) {
        color = weekColor;
    }

    if (select) {
        color = selectTextColor;
    } else if (hover) {
        color = hoverTextColor;
    }

    painter->setPen(color);

    if (showLunar) {
        QFont font;
        font.setPixelSize(side / 2.7);
        painter->setFont(font);

        QRect dayRect = QRect(0, 0, width, height / 1.7);
        painter->drawText(dayRect, Qt::AlignHCenter | Qt::AlignBottom, QString::number(date.day()));
    } else {
        QFont font;
        font.setPixelSize(side / 2);
        painter->setFont(font);

        QRect dayRect = QRect(0, 0, width, height);
        painter->drawText(dayRect, Qt::AlignCenter, QString::number(date.day()));
    }

    painter->restore();
}

void LunarCalendarItem::drawLunar(QPainter *painter)
{
    if (!showLunar) {
        return;
    }

    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    painter->save();

    //判断当前农历文字是否节日,是节日且是当月则用农历节日颜色显示
    bool exist = (!listDayName.contains(lunar) && dayType != DayType_MonthPre && dayType != DayType_MonthNext);

    //根据当前类型选择对应的颜色
    QColor color = currentLunarColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        color = otherLunarColor;
    }

    if (select) {
        color = selectTextColor;
    } else if (hover) {
        color = hoverTextColor;
    } else if (exist) {
        color = lunarColor;
    }

    painter->setPen(color);

    QFont font;
    font.setPixelSize(side / 5);
    painter->setFont(font);

    QRect lunarRect(0, height / 2, width, height / 2);
    painter->drawText(lunarRect, Qt::AlignCenter, lunar);

    painter->restore();
}

QSize LunarCalendarItem::sizeHint() const
{
    return QSize(100, 100);
}

QSize LunarCalendarItem::minimumSizeHint() const
{
    return QSize(20, 20);
}

bool LunarCalendarItem::getSelect() const
{
    return this->select;
}

void LunarCalendarItem::setSelect(bool select)
{
    if (this->select != select) {
        this->select = select;
        this->update();
    }
}

bool LunarCalendarItem::getShowLunar() const
{
    return this->showLunar;
}

void LunarCalendarItem::setShowLunar(bool showLunar)
{
    if (this->showLunar != showLunar) {
        this->showLunar = showLunar;
        this->update();
    }
}

QString LunarCalendarItem::getBgImage() const
{
    return this->bgImage;
}

void LunarCalendarItem::setBgImage(const QString &bgImage)
{
    if (this->bgImage != bgImage) {
        this->bgImage = bgImage;
        this->update();
    }
}

LunarCalendarItem::SelectType LunarCalendarItem::getSelectType() const
{
    return this->selectType;
}

void LunarCalendarItem::setSelectType(const LunarCalendarItem::SelectType &selectType)
{
    if (this->selectType != selectType) {
        this->selectType = selectType;
        this->update();
    }
}

QDate LunarCalendarItem::getDate() const
{
    return this->date;
}

void LunarCalendarItem::setDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        this->update();
    }
}

QString LunarCalendarItem::getLunar() const
{
    return this->lunar;
}

void LunarCalendarItem::setLunar(const QString &lunar)
{
    if (this->lunar != lunar) {
        this->lunar = lunar;
        this->update();
    }
}

LunarCalendarItem::DayType LunarCalendarItem::getDayType() const
{
    return this->dayType;
}

void LunarCalendarItem::setDayType(const LunarCalendarItem::DayType &dayType)
{
    if (this->dayType != dayType) {
        this->dayType = dayType;
        this->update();
    }
}

void LunarCalendarItem::setDate(const QDate &date, const QString &lunar, const DayType &dayType)
{
    this->date = date;
    this->lunar = lunar;
    this->dayType = dayType;
    this->update();
}

QColor LunarCalendarItem::getBorderColor() const
{
    return this->borderColor;
}

void LunarCalendarItem::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        this->update();
    }
}


QColor LunarCalendarItem::getWeekColor() const
{
    return this->weekColor;
}

void LunarCalendarItem::setWeekColor(const QColor &weekColor)
{
    if (this->weekColor != weekColor) {
        this->weekColor = weekColor;
        this->update();
    }
}

QColor LunarCalendarItem::getSuperColor() const
{
    return this->superColor;
}

void LunarCalendarItem::setSuperColor(const QColor &superColor)
{
    if (this->superColor != superColor) {
        this->superColor = superColor;
        this->update();
    }
}

QColor LunarCalendarItem::getLunarColor() const
{
    return this->lunarColor;
}

void LunarCalendarItem::setLunarColor(const QColor &lunarColor)
{
    if (this->lunarColor != lunarColor) {
        this->lunarColor = lunarColor;
        this->update();
    }
}

QColor LunarCalendarItem::getCurrentTextColor() const
{
    return this->currentTextColor;
}

void LunarCalendarItem::setCurrentTextColor(const QColor &currentTextColor)
{
    if (this->currentTextColor != currentTextColor) {
        this->currentTextColor = currentTextColor;
        this->update();
    }
}

QColor LunarCalendarItem::getOtherTextColor() const
{
    return this->otherTextColor;
}

void LunarCalendarItem::setOtherTextColor(const QColor &otherTextColor)
{
    if (this->otherTextColor != otherTextColor) {
        this->otherTextColor = otherTextColor;
        this->update();
    }
}

QColor LunarCalendarItem::getSelectTextColor() const
{
    return this->selectTextColor;
}

void LunarCalendarItem::setSelectTextColor(const QColor &selectTextColor)
{
    if (this->selectTextColor != selectTextColor) {
        this->selectTextColor = selectTextColor;
        this->update();
    }
}

QColor LunarCalendarItem::getHoverTextColor() const
{
    return this->hoverTextColor;
}

void LunarCalendarItem::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        this->update();
    }
}

QColor LunarCalendarItem::getCurrentLunarColor() const
{
    return this->currentLunarColor;
}

void LunarCalendarItem::setCurrentLunarColor(const QColor &currentLunarColor)
{
    if (this->currentLunarColor != currentLunarColor) {
        this->currentLunarColor = currentLunarColor;
        this->update();
    }
}


QColor LunarCalendarItem::getOtherLunarColor() const
{
    return this->otherLunarColor;
}

void LunarCalendarItem::setOtherLunarColor(const QColor &otherLunarColor)
{
    if (this->otherLunarColor != otherLunarColor) {
        this->otherLunarColor = otherLunarColor;
        this->update();
    }
}

QColor LunarCalendarItem::getSelectLunarColor() const
{
    return this->selectLunarColor;
}

void LunarCalendarItem::setSelectLunarColor(const QColor &selectLunarColor)
{
    if (this->selectLunarColor != selectLunarColor) {
        this->selectLunarColor = selectLunarColor;
        this->update();
    }
}

QColor LunarCalendarItem::getHoverLunarColor() const
{
    return this->hoverLunarColor;
}

void LunarCalendarItem::setHoverLunarColor(const QColor &hoverLunarColor)
{
    if (this->hoverLunarColor != hoverLunarColor) {
        this->hoverLunarColor = hoverLunarColor;
        this->update();
    }
}

QColor LunarCalendarItem::getCurrentBgColor() const
{
    return this->currentBgColor;
}

void LunarCalendarItem::setCurrentBgColor(const QColor &currentBgColor)
{
    if (this->currentBgColor != currentBgColor) {
        this->currentBgColor = currentBgColor;
        this->update();
    }
}


QColor LunarCalendarItem::getOtherBgColor() const
{
    return this->otherBgColor;
}

void LunarCalendarItem::setOtherBgColor(const QColor &otherBgColor)
{
    if (this->otherBgColor != otherBgColor) {
        this->otherBgColor = otherBgColor;
        this->update();
    }
}

QColor LunarCalendarItem::getSelectBgColor() const
{
    return this->selectBgColor;
}

void LunarCalendarItem::setSelectBgColor(const QColor &selectBgColor)
{
    if (this->selectBgColor != selectBgColor) {
        this->selectBgColor = selectBgColor;
        this->update();
    }
}

QColor LunarCalendarItem::getHoverBgColor() const
{
    return this->hoverBgColor;
}

void LunarCalendarItem::setHoverBgColor(const QColor &hoverBgColor)
{
    if (this->hoverBgColor != hoverBgColor) {
        this->hoverBgColor = hoverBgColor;
        this->update();
    }
}
