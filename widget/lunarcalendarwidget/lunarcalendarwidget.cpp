#pragma execution_character_set("utf-8")

#include "lunarcalendarwidget.h"
#include "qfontdatabase.h"
#include "qdatetime.h"
#include "qlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include "qcombobox.h"
#include "qdebug.h"

LunarCalendarWidget::LunarCalendarWidget(QWidget *parent) : QWidget(parent)
{
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/font/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    btnClick = false;

    calendarStyle = CalendarStyle_Red;
    weekNameFormat = WeekNameFormat_Short;
    date = QDate::currentDate();

    weekTextColor = QColor(255, 255, 255);
    weekBgColor = QColor(22, 160, 134);

    showLunar = true;
    bgImage = ":/image/bg_calendar.png";
    selectType = SelectType_Rect;

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

    initWidget();
    initStyle();
    initDate();
}

LunarCalendarWidget::~LunarCalendarWidget()
{
}

void LunarCalendarWidget::initWidget()
{
    setObjectName("lunarCalendarWidget");

    //顶部widget
    QWidget *widgetTop = new QWidget;
    widgetTop->setObjectName("widgetTop");
    widgetTop->setMinimumHeight(35);

    //上一年按钮
    QToolButton *btnPrevYear = new QToolButton;
    btnPrevYear->setObjectName("btnPrevYear");
    btnPrevYear->setFixedWidth(35);
    btnPrevYear->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    btnPrevYear->setFont(iconFont);
    btnPrevYear->setText(QChar(0xf060));

    //下一年按钮
    QToolButton *btnNextYear = new QToolButton;
    btnNextYear->setObjectName("btnNextYear");
    btnNextYear->setFixedWidth(35);
    btnNextYear->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    btnNextYear->setFont(iconFont);
    btnNextYear->setText(QChar(0xf061));

    //上个月按钮
    QToolButton *btnPrevMonth = new QToolButton;
    btnPrevMonth->setObjectName("btnPrevMonth");
    btnPrevMonth->setFixedWidth(35);
    btnPrevMonth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnPrevMonth->setFont(iconFont);
    btnPrevMonth->setText(QChar(0xf060));

    //下个月按钮
    QToolButton *btnNextMonth = new QToolButton;
    btnNextMonth->setObjectName("btnNextMonth");
    btnNextMonth->setFixedWidth(35);
    btnNextMonth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnNextMonth->setFont(iconFont);
    btnNextMonth->setText(QChar(0xf061));

    //转到今天
    QPushButton *btnToday = new QPushButton;
    btnToday->setObjectName("btnToday");
    btnToday->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnToday->setText("转到今天");

    //年份下拉框
    cboxYear = new QComboBox;
    cboxYear->setObjectName("cboxYear");
    for (int i = 1901; i <= 2099; ++i) {
        cboxYear->addItem(QString("%1年").arg(i));
    }

    //月份下拉框
    cboxMonth = new QComboBox;
    cboxMonth->setObjectName("cboxMonth");
    for (int i = 1; i <= 12; ++i) {
        cboxMonth->addItem(QString("%1月").arg(i));
    }

    //中间用个空widget隔开
    QWidget *widgetBlank1 = new QWidget;
    widgetBlank1->setFixedWidth(5);
    QWidget *widgetBlank2 = new QWidget;
    widgetBlank2->setFixedWidth(5);

    //顶部横向布局
    QHBoxLayout *layoutTop = new QHBoxLayout(widgetTop);
    layoutTop->setContentsMargins(0, 0, 0, 9);
    layoutTop->addWidget(btnPrevYear);
    layoutTop->addWidget(cboxYear);
    layoutTop->addWidget(btnNextYear);
    layoutTop->addWidget(widgetBlank1);

    layoutTop->addWidget(btnPrevMonth);
    layoutTop->addWidget(cboxMonth);
    layoutTop->addWidget(btnNextMonth);
    layoutTop->addWidget(widgetBlank2);
    layoutTop->addWidget(btnToday);

    //星期widget
    QWidget *widgetWeek = new QWidget;
    widgetWeek->setObjectName("widgetWeek");
    widgetWeek->setMinimumHeight(30);

    //星期布局
    QHBoxLayout *layoutWeek = new QHBoxLayout(widgetWeek);
    layoutWeek->setContentsMargins(0, 0, 0, 0);
    layoutWeek->setSpacing(0);

    for (int i = 0; i < 7; ++i) {
        QLabel *lab = new QLabel;
        lab->setAlignment(Qt::AlignCenter);
        layoutWeek->addWidget(lab);
        labWeeks.append(lab);
    }

    setWeekNameFormat(WeekNameFormat_Long);

    //日期标签widget
    QWidget *widgetBody = new QWidget;
    widgetBody->setObjectName("widgetBody");

    //日期标签布局
    QGridLayout *layoutBody = new QGridLayout(widgetBody);
    layoutBody->setContentsMargins(1, 1, 1, 1);
    layoutBody->setHorizontalSpacing(0);
    layoutBody->setVerticalSpacing(0);

    //逐个添加日标签
    for (int i = 0; i < 42; ++i) {
        LunarCalendarItem *item = new LunarCalendarItem;
        connect(item, SIGNAL(clicked(QDate, LunarCalendarItem::DayType)), this, SLOT(clicked(QDate, LunarCalendarItem::DayType)));
        layoutBody->addWidget(item, i / 7, i % 7);
        items << item;
    }

    //主布局
    QVBoxLayout *verLayoutCalendar = new QVBoxLayout(this);
    verLayoutCalendar->setContentsMargins(0, 0, 0, 0);
    verLayoutCalendar->setSpacing(0);
    verLayoutCalendar->addWidget(widgetTop);
    verLayoutCalendar->addWidget(widgetWeek);
    verLayoutCalendar->addWidget(widgetBody, 1);

    //绑定按钮和下拉框信号
    connect(btnPrevYear, SIGNAL(clicked(bool)), this, SLOT(showPreviousYear()));
    connect(btnNextYear, SIGNAL(clicked(bool)), this, SLOT(showNextYear()));
    connect(btnPrevMonth, SIGNAL(clicked(bool)), this, SLOT(showPreviousMonth()));
    connect(btnNextMonth, SIGNAL(clicked(bool)), this, SLOT(showNextMonth()));
    connect(btnToday, SIGNAL(clicked(bool)), this, SLOT(showToday()));
    connect(cboxYear, SIGNAL(currentIndexChanged(int)), this, SLOT(yearChanged(int)));
    connect(cboxMonth, SIGNAL(currentIndexChanged(int)), this, SLOT(monthChanged(int)));
}

void LunarCalendarWidget::initStyle()
{
    //设置样式
    QStringList qss;

    //星期名称样式
    qss.append(QString("QLabel{background:%1;color:%2;}").arg(weekBgColor.name()).arg(weekTextColor.name()));

    //边框
    qss.append(QString("QWidget#widgetBody{border:1px solid %1;}").arg(borderColor.name()));

    //顶部下拉框
    qss.append(QString("QToolButton{padding:0px;background:none;border:none;border-radius:5px;}"));
    qss.append(QString("QToolButton:hover{background:#16A085;color:#FFFFFF;}"));

    //转到今天
    qss.append(QString("QPushButton{background:#16A085;color:#FFFFFF;border-radius:5px;}"));

    //自定义日控件颜色
    QString strSelectType;
    if (selectType == SelectType_Rect) {
        strSelectType = "SelectType_Rect";
    } else if (selectType == SelectType_Circle) {
        strSelectType = "SelectType_Circle";
    } else if (selectType == SelectType_Triangle) {
        strSelectType = "SelectType_Triangle";
    } else if (selectType == SelectType_Image) {
        strSelectType = "SelectType_Image";
    }

    qss.append(QString("LunarCalendarItem{qproperty-showLunar:%1;}").arg(showLunar));
    qss.append(QString("LunarCalendarItem{qproperty-bgImage:%1;}").arg(bgImage));
    qss.append(QString("LunarCalendarItem{qproperty-selectType:%1;}").arg(strSelectType));
    qss.append(QString("LunarCalendarItem{qproperty-borderColor:%1;}").arg(borderColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-weekColor:%1;}").arg(weekColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-superColor:%1;}").arg(superColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-lunarColor:%1;}").arg(lunarColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-currentTextColor:%1;}").arg(currentTextColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-otherTextColor:%1;}").arg(otherTextColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-selectTextColor:%1;}").arg(selectTextColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-hoverTextColor:%1;}").arg(hoverTextColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-currentLunarColor:%1;}").arg(currentLunarColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-otherLunarColor:%1;}").arg(otherLunarColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-selectLunarColor:%1;}").arg(selectLunarColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-hoverLunarColor:%1;}").arg(hoverLunarColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-currentBgColor:%1;}").arg(currentBgColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-otherBgColor:%1;}").arg(otherBgColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-selectBgColor:%1;}").arg(selectBgColor.name()));
    qss.append(QString("LunarCalendarItem{qproperty-hoverBgColor:%1;}").arg(hoverBgColor.name()));

    this->setStyleSheet(qss.join(""));
}

//初始化日期面板
void LunarCalendarWidget::initDate()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();

    //设置为今天,设置变量防止重复触发
    btnClick = true;
    cboxYear->setCurrentIndex(cboxYear->findText(QString("%1年").arg(year)));
    cboxMonth->setCurrentIndex(cboxMonth->findText(QString("%1月").arg(month)));
    btnClick = false;

    //首先判断当前月的第一天是星期几
    int week = LunarCalendarInfo::Instance()->getFirstDayOfWeek(year, month);
    //当前月天数
    int countDay = LunarCalendarInfo::Instance()->getMonthDays(year, month);
    //上月天数
    int countDayPre = LunarCalendarInfo::Instance()->getMonthDays(1 == month ? year - 1 : year, 1 == month ? 12 : month - 1);

    //如果上月天数上月刚好一周则另外处理
    int startPre, endPre, startNext, endNext, index, tempYear, tempMonth, tempDay;
    if (0 == week) {
        startPre = 0;
        endPre = 7;
        startNext = 0;
        endNext = 42 - (countDay + 7);
    } else {
        startPre = 0;
        endPre = week;
        startNext = week + countDay;
        endNext = 42;
    }

    //纠正1月份前面部分偏差,1月份前面部分是上一年12月份
    tempYear = year;
    tempMonth = month - 1;
    if (tempMonth < 1) {
        tempYear--;
        tempMonth = 12;
    }

    //显示上月天数
    for (int i = startPre; i < endPre; ++i) {
        index = i;
        tempDay = countDayPre - endPre + i + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        items.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthPre);
    }

    //纠正12月份后面部分偏差,12月份后面部分是下一年1月份
    tempYear = year;
    tempMonth = month + 1;
    if (tempMonth > 12) {
        tempYear++;
        tempMonth = 1;
    }

    //显示下月天数
    for (int i = startNext; i < endNext; ++i) {
        index = 42 - endNext + i;
        tempDay = i - startNext + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        items.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthNext);
    }

    //重新置为当前年月
    tempYear = year;
    tempMonth = month;

    //显示当前月
    for (int i = week; i < (countDay + week); ++i) {
        index = (0 == week ? (i + 7) : i);
        tempDay = i - week + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        if (0 == (i % 7) || 6 == (i % 7)) {
            items.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_WeekEnd);
        } else {
            items.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthCurrent);
        }
    }

    dayChanged(this->date);
}

void LunarCalendarWidget::yearChanged(int)
{
    //如果是单击按钮切换的日期变动则不需要触发
    if (btnClick) {
        return;
    }

    QString arg1 = cboxYear->currentText();
    int year = arg1.mid(0, arg1.length() - 1).toInt();
    int month = date.month();
    int day = date.day();
    dateChanged(year, month, day);
}

void LunarCalendarWidget::monthChanged(int)
{
    //如果是单击按钮切换的日期变动则不需要触发
    if (btnClick) {
        return;
    }

    QString arg1 = cboxMonth->currentText();
    int year = date.year();
    int month = arg1.mid(0, arg1.length() - 1).toInt();
    int day = date.day();
    dateChanged(year, month, day);
}

void LunarCalendarWidget::clicked(const QDate &date, const LunarCalendarItem::DayType &dayType)
{
    if (LunarCalendarItem::DayType_MonthPre == dayType) {
        showPreviousMonth();
    } else if (LunarCalendarItem::DayType_MonthNext == dayType) {
        showNextMonth();
    } else {
        this->date = date;
        dayChanged(this->date);
    }
}

void LunarCalendarWidget::dayChanged(const QDate &date)
{
    //计算星期几,当前天对应标签索引=日期+星期几-1
    int year = date.year();
    int month = date.month();
    int day = date.day();
    int week = LunarCalendarInfo::Instance()->getFirstDayOfWeek(year, month);
    //qDebug() << QString("%1-%2-%3").arg(year).arg(month).arg(day);

    //选中当前日期,其他日期恢复,这里还有优化空间,比方说类似单选框机制
    for (int i = 0; i < 42; ++i) {
        //当月第一天是星期天要另外计算
        int index = day + week - 1;
        if (week == 0) {
            index = day + 6;
        }

        items.at(i)->setSelect(i == index);
    }

    //发送日期单击信号
    emit clicked(date);
    //发送日期更新信号
    emit selectionChanged();
}

void LunarCalendarWidget::dateChanged(int year, int month, int day)
{
    //如果原有天大于28则设置为1,防止出错
    date.setDate(year, month, day > 28 ? 1 : day);
    initDate();
}

QSize LunarCalendarWidget::sizeHint() const
{
    return QSize(600, 500);
}

QSize LunarCalendarWidget::minimumSizeHint() const
{
    return QSize(200, 150);
}

LunarCalendarWidget::CalendarStyle LunarCalendarWidget::getCalendarStyle() const
{
    return this->calendarStyle;
}

void LunarCalendarWidget::setCalendarStyle(const LunarCalendarWidget::CalendarStyle &calendarStyle)
{
    if (this->calendarStyle != calendarStyle) {
        this->calendarStyle = calendarStyle;
    }
}

LunarCalendarWidget::WeekNameFormat LunarCalendarWidget::getWeekNameFormat() const
{
    return this->weekNameFormat;
}

void LunarCalendarWidget::setWeekNameFormat(const LunarCalendarWidget::WeekNameFormat &weekNameFormat)
{
    if (this->weekNameFormat != weekNameFormat) {
        this->weekNameFormat = weekNameFormat;

        QStringList listWeek;
        if (weekNameFormat == WeekNameFormat_Short) {
            listWeek << "日" << "一" << "二" << "三" << "四" << "五" << "六";
        } else if (weekNameFormat == WeekNameFormat_Normal) {
            listWeek << "周日" << "周一" << "周二" << "周三" << "周四" << "周五" << "周六";
        } else if (weekNameFormat == WeekNameFormat_Long) {
            listWeek << "星期天" << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六";
        } else if (weekNameFormat == WeekNameFormat_En) {
            listWeek << "Sun" << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat";
        }

        //逐个添加日期文字
        for (int i = 0; i < 7; ++i) {
            labWeeks.at(i)->setText(listWeek.at(i));
        }
    }
}

QDate LunarCalendarWidget::getDate() const
{
    return this->date;
}

void LunarCalendarWidget::setDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        initDate();
    }
}

QColor LunarCalendarWidget::getWeekTextColor() const
{
    return this->weekTextColor;
}

void LunarCalendarWidget::setWeekTextColor(const QColor &weekTextColor)
{
    if (this->weekTextColor != weekTextColor) {
        this->weekTextColor = weekTextColor;
        this->initStyle();
    }
}


QColor LunarCalendarWidget::getWeekBgColor() const
{
    return this->weekBgColor;
}

void LunarCalendarWidget::setWeekBgColor(const QColor &weekBgColor)
{
    if (this->weekBgColor != weekBgColor) {
        this->weekBgColor = weekBgColor;
        this->initStyle();
    }
}

bool LunarCalendarWidget::getShowLunar() const
{
    return this->showLunar;
}

void LunarCalendarWidget::setShowLunar(bool showLunar)
{
    if (this->showLunar != showLunar) {
        this->showLunar = showLunar;
        this->initStyle();
    }
}

QString LunarCalendarWidget::getBgImage() const
{
    return this->bgImage;
}

void LunarCalendarWidget::setBgImage(const QString &bgImage)
{
    if (this->bgImage != bgImage) {
        this->bgImage = bgImage;
        this->initStyle();
    }
}

LunarCalendarWidget::SelectType LunarCalendarWidget::getSelectType() const
{
    return this->selectType;
}

void LunarCalendarWidget::setSelectType(const LunarCalendarWidget::SelectType &selectType)
{
    if (this->selectType != selectType) {
        this->selectType = selectType;
        this->initStyle();
    }
}


QColor LunarCalendarWidget::getBorderColor() const
{
    return this->borderColor;
}

void LunarCalendarWidget::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getWeekColor() const
{
    return this->weekColor;
}

void LunarCalendarWidget::setWeekColor(const QColor &weekColor)
{
    if (this->weekColor != weekColor) {
        this->weekColor = weekColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getSuperColor() const
{
    return this->superColor;
}

void LunarCalendarWidget::setSuperColor(const QColor &superColor)
{
    if (this->superColor != superColor) {
        this->superColor = superColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getLunarColor() const
{
    return this->lunarColor;
}

void LunarCalendarWidget::setLunarColor(const QColor &lunarColor)
{
    if (this->lunarColor != lunarColor) {
        this->lunarColor = lunarColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getCurrentTextColor() const
{
    return this->currentTextColor;
}

void LunarCalendarWidget::setCurrentTextColor(const QColor &currentTextColor)
{
    if (this->currentTextColor != currentTextColor) {
        this->currentTextColor = currentTextColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getOtherTextColor() const
{
    return this->otherTextColor;
}

void LunarCalendarWidget::setOtherTextColor(const QColor &otherTextColor)
{
    if (this->otherTextColor != otherTextColor) {
        this->otherTextColor = otherTextColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getSelectTextColor() const
{
    return this->selectTextColor;
}

void LunarCalendarWidget::setSelectTextColor(const QColor &selectTextColor)
{
    if (this->selectTextColor != selectTextColor) {
        this->selectTextColor = selectTextColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getHoverTextColor() const
{
    return this->hoverTextColor;
}

void LunarCalendarWidget::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getCurrentLunarColor() const
{
    return this->currentLunarColor;
}

void LunarCalendarWidget::setCurrentLunarColor(const QColor &currentLunarColor)
{
    if (this->currentLunarColor != currentLunarColor) {
        this->currentLunarColor = currentLunarColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getOtherLunarColor() const
{
    return this->otherLunarColor;
}

void LunarCalendarWidget::setOtherLunarColor(const QColor &otherLunarColor)
{
    if (this->otherLunarColor != otherLunarColor) {
        this->otherLunarColor = otherLunarColor;
        this->initStyle();
    }
}


QColor LunarCalendarWidget::getSelectLunarColor() const
{
    return this->selectLunarColor;
}

void LunarCalendarWidget::setSelectLunarColor(const QColor &selectLunarColor)
{
    if (this->selectLunarColor != selectLunarColor) {
        this->selectLunarColor = selectLunarColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getHoverLunarColor() const
{
    return this->hoverLunarColor;
}

void LunarCalendarWidget::setHoverLunarColor(const QColor &hoverLunarColor)
{
    if (this->hoverLunarColor != hoverLunarColor) {
        this->hoverLunarColor = hoverLunarColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getCurrentBgColor() const
{
    return this->currentBgColor;
}

void LunarCalendarWidget::setCurrentBgColor(const QColor &currentBgColor)
{
    if (this->currentBgColor != currentBgColor) {
        this->currentBgColor = currentBgColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getOtherBgColor() const
{
    return this->otherBgColor;
}

void LunarCalendarWidget::setOtherBgColor(const QColor &otherBgColor)
{
    if (this->otherBgColor != otherBgColor) {
        this->otherBgColor = otherBgColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getSelectBgColor() const
{
    return this->selectBgColor;
}

void LunarCalendarWidget::setSelectBgColor(const QColor &selectBgColor)
{
    if (this->selectBgColor != selectBgColor) {
        this->selectBgColor = selectBgColor;
        this->initStyle();
    }
}

QColor LunarCalendarWidget::getHoverBgColor() const
{
    return this->hoverBgColor;
}

void LunarCalendarWidget::setHoverBgColor(const QColor &hoverBgColor)
{
    if (this->hoverBgColor != hoverBgColor) {
        this->hoverBgColor = hoverBgColor;
        this->initStyle();
    }
}

//显示上一年
void LunarCalendarWidget::showPreviousYear()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year <= 1901) {
        return;
    }

    year--;
    dateChanged(year, month, day);
}

//显示下一年
void LunarCalendarWidget::showNextYear()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year >= 2099) {
        return;
    }

    year++;
    dateChanged(year, month, day);
}

//显示上月日期
void LunarCalendarWidget::showPreviousMonth()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year <= 1901 && month == 1) {
        return;
    }

    month--;
    if (month < 1) {
        month = 12;
        year--;
    }

    dateChanged(year, month, day);
}

//显示下月日期
void LunarCalendarWidget::showNextMonth()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year >= 2099 && month == 12) {
        return;
    }

    month++;
    if (month > 12) {
        month = 1;
        year++;
    }

    dateChanged(year, month, day);
}

//转到今天
void LunarCalendarWidget::showToday()
{
    date = QDate::currentDate();
    initDate();
    dayChanged(date);
}
