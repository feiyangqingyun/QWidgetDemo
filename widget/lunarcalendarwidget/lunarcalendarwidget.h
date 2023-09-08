#ifndef LUNARCALENDARWIDGET_H
#define LUNARCALENDARWIDGET_H

/**
 * 自定义农历控件 作者:倪大侠 整理:feiyangqingyun(QQ:517216493) 2017-11-17
 * 1. 可设置边框颜色、周末颜色、角标颜色、农历节日颜色。
 * 2. 可设置当前月文字颜色、其他月文字颜色、选中日期文字颜色、悬停日期文字颜色。
 * 3. 可设置当前月农历文字颜色、其他月农历文字颜色、选中日期农历文字颜色、悬停日期农历文字颜色。
 * 4. 可设置当前月背景颜色、其他月背景颜色、选中日期背景颜色、悬停日期背景颜色。
 * 5. 可设置三种选中背景模式，矩形背景、圆形背景、图片背景。
 * 6. 可直接切换到上一年、下一年、上一月、下一月、转到今天。
 * 7. 可设置是否显示农历信息，不显示则当做正常的日历使用。
 * 8. 支持1901年-2099年范围。
 * 9. 很方便改成多选日期。
 */

#include <QWidget>
#include <QDate>

#include "lunarcalendarinfo.h"
#include "lunarcalendaritem.h"

class QLabel;
class QComboBox;
class LunarCalendarItem;

#ifdef quc
class Q_DECL_EXPORT LunarCalendarWidget : public QWidget
#else
class LunarCalendarWidget : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(CalendarStyle)
    Q_ENUMS(WeekNameFormat)
    Q_ENUMS(SelectType)

    Q_PROPERTY(CalendarStyle calendarStyle READ getCalendarStyle WRITE setCalendarStyle)
    Q_PROPERTY(WeekNameFormat weekNameFormat READ getWeekNameFormat WRITE setWeekNameFormat)
    Q_PROPERTY(QDate date READ getDate WRITE setDate)

    Q_PROPERTY(QColor weekTextColor READ getWeekTextColor WRITE setWeekTextColor)
    Q_PROPERTY(QColor weekBgColor READ getWeekBgColor WRITE setWeekBgColor)

    Q_PROPERTY(bool showLunar READ getShowLunar WRITE setShowLunar)
    Q_PROPERTY(QString bgImage READ getBgImage WRITE setBgImage)
    Q_PROPERTY(SelectType selectType READ getSelectType WRITE setSelectType)

    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(QColor weekColor READ getWeekColor WRITE setWeekColor)
    Q_PROPERTY(QColor superColor READ getSuperColor WRITE setSuperColor)
    Q_PROPERTY(QColor lunarColor READ getLunarColor WRITE setLunarColor)

    Q_PROPERTY(QColor currentTextColor READ getCurrentTextColor WRITE setCurrentTextColor)
    Q_PROPERTY(QColor otherTextColor READ getOtherTextColor WRITE setOtherTextColor)
    Q_PROPERTY(QColor selectTextColor READ getSelectTextColor WRITE setSelectTextColor)
    Q_PROPERTY(QColor hoverTextColor READ getHoverTextColor WRITE setHoverTextColor)

    Q_PROPERTY(QColor currentLunarColor READ getCurrentLunarColor WRITE setCurrentLunarColor)
    Q_PROPERTY(QColor otherLunarColor READ getOtherLunarColor WRITE setOtherLunarColor)
    Q_PROPERTY(QColor selectLunarColor READ getSelectLunarColor WRITE setSelectLunarColor)
    Q_PROPERTY(QColor hoverLunarColor READ getHoverLunarColor WRITE setHoverLunarColor)

    Q_PROPERTY(QColor currentBgColor READ getCurrentBgColor WRITE setCurrentBgColor)
    Q_PROPERTY(QColor otherBgColor READ getOtherBgColor WRITE setOtherBgColor)
    Q_PROPERTY(QColor selectBgColor READ getSelectBgColor WRITE setSelectBgColor)
    Q_PROPERTY(QColor hoverBgColor READ getHoverBgColor WRITE setHoverBgColor)

public:
    enum CalendarStyle {
        CalendarStyle_Red = 0
    };

    enum WeekNameFormat {
        WeekNameFormat_Short = 0,   //短名称
        WeekNameFormat_Normal = 1,  //普通名称
        WeekNameFormat_Long = 2,    //长名称
        WeekNameFormat_En = 3       //英文名称
    };

    enum SelectType {
        SelectType_Rect = 0,        //矩形背景
        SelectType_Circle = 1,      //圆形背景
        SelectType_Triangle = 2,    //带三角标
        SelectType_Image = 3        //图片背景
    };

    explicit LunarCalendarWidget(QWidget *parent = 0);
    ~LunarCalendarWidget();

private:
    QFont iconFont;                 //图形字体
    bool btnClick;                  //按钮单击,避开下拉选择重复触发
    QComboBox *cboxYear;            //年份下拉框
    QComboBox *cboxMonth;           //月份下拉框
    QList<QLabel *> labWeeks;       //顶部星期名称
    QList<LunarCalendarItem *> items;//日期元素

    CalendarStyle calendarStyle;    //整体样式
    WeekNameFormat weekNameFormat;  //星期名称格式
    QDate date;                     //当前日期

    QColor weekTextColor;           //星期名称文字颜色
    QColor weekBgColor;             //星期名称背景色

    bool showLunar;                 //显示农历
    QString bgImage;                //背景图片
    SelectType selectType;          //选中模式

    QColor borderColor;             //边框颜色
    QColor weekColor;               //周末颜色
    QColor superColor;              //角标颜色
    QColor lunarColor;              //农历节日颜色

    QColor currentTextColor;        //当前月文字颜色
    QColor otherTextColor;          //其他月文字颜色
    QColor selectTextColor;         //选中日期文字颜色
    QColor hoverTextColor;          //悬停日期文字颜色

    QColor currentLunarColor;       //当前月农历文字颜色
    QColor otherLunarColor;         //其他月农历文字颜色
    QColor selectLunarColor;        //选中日期农历文字颜色
    QColor hoverLunarColor;         //悬停日期农历文字颜色

    QColor currentBgColor;          //当前月背景颜色
    QColor otherBgColor;            //其他月背景颜色
    QColor selectBgColor;           //选中日期背景颜色
    QColor hoverBgColor;            //悬停日期背景颜色

private slots:
    void initWidget();
    void initStyle();
    void initDate();
    void yearChanged(int);
    void monthChanged(int);
    void clicked(const QDate &date, const LunarCalendarItem::DayType &dayType);
    void dayChanged(const QDate &date);
    void dateChanged(int year, int month, int day);

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置整体样式
    CalendarStyle getCalendarStyle() const;
    void setCalendarStyle(const CalendarStyle &calendarStyle);

    //获取和设置星期名称格式
    WeekNameFormat getWeekNameFormat() const;
    void setWeekNameFormat(const WeekNameFormat &weekNameFormat);

    //获取和设置日期
    QDate getDate() const;
    void setDate(const QDate &date);

    //获取和设置顶部星期名称文字颜色
    QColor getWeekTextColor() const;
    void setWeekTextColor(const QColor &weekTextColor);

    //获取和设置顶部星期名称文字背景色
    QColor getWeekBgColor() const;
    void setWeekBgColor(const QColor &weekBgColor);

    //获取和设置是否显示农历信息
    bool getShowLunar() const;
    void setShowLunar(bool showLunar);

    //获取和设置背景图片
    QString getBgImage() const;
    void setBgImage(const QString &bgImage);

    //获取和设置选中背景样式
    SelectType getSelectType() const;
    void setSelectType(const SelectType &selectType);

    //获取和设置边框颜色
    QColor getBorderColor() const;
    void setBorderColor(const QColor &borderColor);

    //获取和设置周末颜色
    QColor getWeekColor() const;
    void setWeekColor(const QColor &weekColor);

    //获取和设置角标颜色
    QColor getSuperColor() const;
    void setSuperColor(const QColor &superColor);

    //获取和设置农历节日颜色
    QColor getLunarColor() const;
    void setLunarColor(const QColor &lunarColor);

    //获取和设置当前月文字颜色
    QColor getCurrentTextColor() const;
    void setCurrentTextColor(const QColor &currentTextColor);

    //获取和设置其他月文字颜色
    QColor getOtherTextColor() const;
    void setOtherTextColor(const QColor &otherTextColor);

    //获取和设置选中日期文字颜色
    QColor getSelectTextColor() const;
    void setSelectTextColor(const QColor &selectTextColor);

    //获取和设置悬停日期文字颜色
    QColor getHoverTextColor() const;
    void setHoverTextColor(const QColor &hoverTextColor);

    //获取和设置当前月农历文字颜色
    QColor getCurrentLunarColor() const;
    void setCurrentLunarColor(const QColor &currentLunarColor);

    //获取和设置其他月农历文字颜色
    QColor getOtherLunarColor() const;
    void setOtherLunarColor(const QColor &otherLunarColor);

    //获取和设置选中日期农历文字颜色
    QColor getSelectLunarColor() const;
    void setSelectLunarColor(const QColor &selectLunarColor);

    //获取和设置悬停日期农历文字颜色
    QColor getHoverLunarColor() const;
    void setHoverLunarColor(const QColor &hoverLunarColor);

    //获取和设置当前月背景颜色
    QColor getCurrentBgColor() const;
    void setCurrentBgColor(const QColor &currentBgColor);

    //获取和设置其他月背景颜色
    QColor getOtherBgColor() const;
    void setOtherBgColor(const QColor &otherBgColor);

    //获取和设置选中日期背景颜色
    QColor getSelectBgColor() const;
    void setSelectBgColor(const QColor &selectBgColor);

    //获取和设置悬停日期背景颜色
    QColor getHoverBgColor() const;
    void setHoverBgColor(const QColor &hoverBgColor);

public Q_SLOTS:
    //转到上一年
    void showPreviousYear();
    //转到下一年
    void showNextYear();

    //转到上一月
    void showPreviousMonth();
    //转到下一月
    void showNextMonth();

    //转到今天
    void showToday();

Q_SIGNALS:
    void clicked(const QDate &date);
    void selectionChanged();
};

#endif // LUNARCALENDARWIDGET_H
