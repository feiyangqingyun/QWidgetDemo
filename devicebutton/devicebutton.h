#ifndef DEVICEBUTTON_H
#define DEVICEBUTTON_H

/**
 * 设备按钮控件 作者:feiyangqingyun(QQ:517216493) 2018-7-2
 * 1. 可设置按钮样式 圆形、警察、气泡、气泡2、消息、消息2。
 * 2. 可设置按钮颜色 布防、撤防、报警、旁路、故障。
 * 3. 可设置报警切换。
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
        ButtonColor_Green = 0,      //绿色 激活状态
        ButtonColor_Blue = 1,       //蓝色 在线状态
        ButtonColor_Red = 2,        //红色 报警状态
        ButtonColor_Gray = 3,       //灰色 离线状态
        ButtonColor_Black = 4,      //黑色 故障状态
        ButtonColor_Purple = 5,     //紫色 其他状态
        ButtonColor_Yellow = 6      //黄色 其他状态
    };

    explicit DeviceButton(QWidget *parent = 0);
    ~DeviceButton();

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool canMove;                   //是否可移动
    QString text;                   //显示文字
    ButtonStyle buttonStyle;        //按钮样式
    ButtonColor buttonColor;        //按钮颜色

    QString type;                   //图片末尾类型
    QString imgName;                //背景图片名称
    bool isDark;                    //是否加深报警
    QTimer *timer;                  //报警闪烁定时器

private slots:
    void checkAlarm();              //切换报警状态

public:
    bool getCanMove()               const;
    QString getText()               const;

    ButtonStyle getButtonStyle()    const;
    ButtonColor getButtonColor()    const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    //设置可移动
    void setCanMove(bool canMove);
    //设置显示文字
    void setText(const QString &text);
    //设置样式
    void setButtonStyle(const ButtonStyle &buttonStyle);
    //设置颜色
    void setButtonColor(const ButtonColor &buttonColor);

Q_SIGNALS:
    void clicked();
    void doubleClicked();
};

#endif //DEVICEBUTTON_H
