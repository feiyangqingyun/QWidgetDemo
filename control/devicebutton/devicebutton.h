#ifndef DEVICEBUTTON_H
#define DEVICEBUTTON_H

/**
 * 设备按钮控件 作者:feiyangqingyun(QQ:517216493) 2018-07-02
 * 1. 可设置按钮样式 圆形、警察、气泡、气泡2、消息、消息2。
 * 2. 可设置按钮颜色 布防、撤防、报警、旁路、故障。
 * 3. 可设置报警切换及对应报警切换的颜色。
 * 4. 可设置显示的防区号。
 * 5. 可设置是否可鼠标拖动。
 * 6. 发出单击和双击信号。
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT DeviceButton : public QWidget
#else
class DeviceButton : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(ButtonStyle)
    Q_ENUMS(ButtonColor)

    Q_PROPERTY(bool canMove READ getCanMove WRITE setCanMove)
    Q_PROPERTY(QString text READ getText WRITE setText)

    Q_PROPERTY(QString colorNormal READ getColorNormal WRITE setColorNormal)
    Q_PROPERTY(QString colorAlarm READ getColorAlarm WRITE setColorAlarm)

    Q_PROPERTY(ButtonStyle buttonStyle READ getButtonStyle WRITE setButtonStyle)
    Q_PROPERTY(ButtonColor buttonColor READ getButtonColor WRITE setButtonColor)

public:
    //设备按钮样式
    enum ButtonStyle {
        ButtonStyle_Circle = 0,     //圆形
        ButtonStyle_Police = 1,     //警察
        ButtonStyle_Bubble = 2,     //气泡
        ButtonStyle_Bubble2 = 3,    //气泡2
        ButtonStyle_Msg = 4,        //消息
        ButtonStyle_Msg2 = 5        //消息2
    };

    //设备按钮颜色
    enum ButtonColor {
        ButtonColor_Green = 0,      //绿色(激活状态)
        ButtonColor_Blue = 1,       //蓝色(在线状态)
        ButtonColor_Red = 2,        //红色(报警状态)
        ButtonColor_Gray = 3,       //灰色(离线状态)
        ButtonColor_Black = 4,      //黑色(故障状态)
        ButtonColor_Purple = 5,     //紫色(其他状态)
        ButtonColor_Yellow = 6      //黄色(其他状态)
    };

    explicit DeviceButton(QWidget *parent = 0);
    ~DeviceButton();

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool canMove;           //是否可移动
    QString text;           //显示文字

    QString colorNormal;    //正常颜色
    QString colorAlarm;     //报警颜色

    ButtonStyle buttonStyle;//按钮样式
    ButtonColor buttonColor;//按钮颜色

    bool isPressed;         //鼠标是否按下
    QPoint lastPoint;       //鼠标按下最后坐标

    QString type;           //图片末尾类型
    QString imgPath;        //背景图片路径
    QString imgName;        //背景图片名称

    bool isDark;            //是否加深报警
    QTimer *timer;          //报警闪烁定时器

private slots:
    void checkAlarm();      //切换报警状态

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置可移动
    bool getCanMove() const;
    void setCanMove(bool canMove);

    //获取和设置显示文字
    QString getText() const;
    void setText(const QString &text);

    //获取和设置正常颜色
    QString getColorNormal() const;
    void setColorNormal(const QString &colorNormal);

    //获取和设置报警颜色
    QString getColorAlarm() const;
    void setColorAlarm(const QString &colorAlarm);

    //获取和设置样式
    ButtonStyle getButtonStyle() const;
    void setButtonStyle(const ButtonStyle &buttonStyle);

    //获取和设置颜色
    ButtonColor getButtonColor() const;
    void setButtonColor(const ButtonColor &buttonColor);

Q_SIGNALS:
    //鼠标单击双击事件
    void clicked();
    void doubleClicked();
};

#endif //DEVICEBUTTON_H
