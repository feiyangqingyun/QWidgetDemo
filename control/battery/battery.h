#ifndef BATTERY_H
#define BATTERY_H

/**
 * 电池电量控件 作者:feiyangqingyun(QQ:517216493) 2016-10-23
 * 1. 可设置电池电量，动态切换电池电量变化。
 * 2. 可设置电池电量警戒值。
 * 3. 可设置电池电量正常颜色和报警颜色。
 * 4. 可设置边框渐变颜色。
 * 5. 可设置电量变化时每次移动的步长。
 * 6. 可设置边框圆角角度、背景进度圆角角度、头部圆角角度。
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT Battery : public QWidget
#else
class Battery : public QWidget
#endif

{
    Q_OBJECT

    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(double value READ getValue WRITE setValue)
    Q_PROPERTY(double alarmValue READ getAlarmValue WRITE setAlarmValue)

    Q_PROPERTY(bool animation READ getAnimation WRITE setAnimation)
    Q_PROPERTY(double animationStep READ getAnimationStep WRITE setAnimationStep)

    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius)
    Q_PROPERTY(int bgRadius READ getBgRadius WRITE setBgRadius)
    Q_PROPERTY(int headRadius READ getHeadRadius WRITE setHeadRadius)

    Q_PROPERTY(QColor borderColorStart READ getBorderColorStart WRITE setBorderColorStart)
    Q_PROPERTY(QColor borderColorEnd READ getBorderColorEnd WRITE setBorderColorEnd)

    Q_PROPERTY(QColor alarmColorStart READ getAlarmColorStart WRITE setAlarmColorStart)
    Q_PROPERTY(QColor alarmColorEnd READ getAlarmColorEnd WRITE setAlarmColorEnd)

    Q_PROPERTY(QColor normalColorStart READ getNormalColorStart WRITE setNormalColorStart)
    Q_PROPERTY(QColor normalColorEnd READ getNormalColorEnd WRITE setNormalColorEnd)

public:
    explicit Battery(QWidget *parent = 0);
    ~Battery();

protected:
    void paintEvent(QPaintEvent *);
    void drawBorder(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawHead(QPainter *painter);

private slots:
    void updateValue();

private:
    double minValue;        //最小值
    double maxValue;        //最大值
    double value;           //目标电量
    double alarmValue;      //电池电量警戒值

    bool animation;         //是否启用动画显示
    double animationStep;   //动画显示时步长

    int borderWidth;        //边框粗细
    int borderRadius;       //边框圆角角度
    int bgRadius;           //背景进度圆角角度
    int headRadius;         //头部圆角角度

    QColor borderColorStart;//边框渐变开始颜色
    QColor borderColorEnd;  //边框渐变结束颜色

    QColor alarmColorStart; //电池低电量时的渐变开始颜色
    QColor alarmColorEnd;   //电池低电量时的渐变结束颜色

    QColor normalColorStart;//电池正常电量时的渐变开始颜色
    QColor normalColorEnd;  //电池正常电量时的渐变结束颜色

    bool isForward;         //是否往前移
    double currentValue;    //当前电量
    QRectF batteryRect;     //电池主体区域
    QTimer *timer;          //绘制定时器

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //设置范围值
    void setRange(double minValue, double maxValue);
    void setRange(int minValue, int maxValue);

    //获取和设置最小值
    double getMinValue() const;
    void setMinValue(double minValue);

    //获取和设置最大值
    double getMaxValue() const;
    void setMaxValue(double maxValue);

    //获取和设置电池电量值
    double getValue() const;
    void setValue(double value);

    //获取和设置电池电量警戒值
    double getAlarmValue() const;
    void setAlarmValue(double alarmValue);

    //获取和设置是否启用动画显示
    bool getAnimation() const;
    void setAnimation(bool animation);

    //获取和设置动画显示的步长
    double getAnimationStep() const;
    void setAnimationStep(double animationStep);

    //获取和设置边框粗细
    int getBorderWidth() const;
    void setBorderWidth(int borderWidth);

    //获取和设置边框圆角角度
    int getBorderRadius() const;
    void setBorderRadius(int borderRadius);

    //获取和设置背景圆角角度
    int getBgRadius() const;
    void setBgRadius(int bgRadius);

    //获取和设置头部圆角角度
    int getHeadRadius() const;
    void setHeadRadius(int headRadius);

    //获取和设置边框渐变颜色
    QColor getBorderColorStart() const;
    void setBorderColorStart(const QColor &borderColorStart);

    QColor getBorderColorEnd() const;
    void setBorderColorEnd(const QColor &borderColorEnd);

    //获取和设置电池电量报警时的渐变颜色
    QColor getAlarmColorStart() const;
    void setAlarmColorStart(const QColor &alarmColorStart);

    QColor getAlarmColorEnd() const;
    void setAlarmColorEnd(const QColor &alarmColorEnd);

    //获取和设置电池电量正常时的渐变颜色
    QColor getNormalColorStart() const;
    void setNormalColorStart(const QColor &normalColorStart);

    QColor getNormalColorEnd() const;
    void setNormalColorEnd(const QColor &normalColorEnd);

public Q_SLOTS:
    void setValue(int value);
    void setAlarmValue(int alarmValue);

Q_SIGNALS:
    void valueChanged(double value);
};

#endif // BATTERY_H
