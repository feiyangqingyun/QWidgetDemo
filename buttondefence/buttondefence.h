#ifndef BUTTONDEFENCE_H
#define BUTTONDEFENCE_H

/**
 * 防区按钮控件 作者:feiyangqingyun(QQ:517216493) 2018-7-2
 * 1. 可设置防区样式  圆形、警察、气泡、气泡2、消息、消息2
 * 2. 可设置防区状态  布防、撤防、报警、旁路、故障
 * 3. 可设置报警切换
 * 4. 可设置显示的防区号
 * 5. 可设置是否可鼠标拖动
 * 6. 发出单击和双击信号
 */

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT ButtonDefence : public QWidget
#else
class ButtonDefence : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(ButtonStyle)
    Q_ENUMS(ButtonStatus)

    Q_PROPERTY(bool canMove READ getCanMove WRITE setCanMove)
    Q_PROPERTY(QString text READ getText WRITE setText)

    Q_PROPERTY(ButtonStyle buttonStyle READ getButtonStyle WRITE setButtonStyle)
    Q_PROPERTY(ButtonStatus buttonStatus READ getButtonStatus WRITE setButtonStatus)

public:
    //防区样式  圆形、警察、气泡、气泡2、消息、消息2、自定义
    //如果设置的自定义的，则图片拓展名 btn_defence_alarm_custom
    enum ButtonStyle {
        ButtonStyle_Circle = 0,
        ButtonStyle_Police = 1,
        ButtonStyle_Bubble = 2,
        ButtonStyle_Bubble2 = 3,
        ButtonStyle_Msg = 4,
        ButtonStyle_Msg2 = 5,
        ButtonStyle_Custom = 6
    };

    //防区状态  布防、撤防、报警、旁路、故障
    enum ButtonStatus {
        ButtonStatus_Arming = 0,
        ButtonStatus_Disarming = 1,
        ButtonStatus_Alarm = 2,
        ButtonStatus_Bypass = 3,
        ButtonStatus_Error = 4
    };

    explicit ButtonDefence(QWidget *parent = 0);
    ~ButtonDefence();

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool canMove;                   //是否可移动
    QString text;                   //显示文字
    ButtonStyle buttonStyle;        //防区样式
    ButtonStatus buttonStatus;      //防区状态

    QString type;                   //图片末尾类型
    QString imgName;                //背景图片名称
    bool isDark;                    //是否加深报警
    QTimer *timer;                  //报警闪烁定时器

private slots:
    void checkAlarm();

public:
    bool getCanMove()               const;
    QString getText()               const;

    ButtonStyle getButtonStyle()    const;
    ButtonStatus getButtonStatus()  const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    //设置可移动
    void setCanMove(bool canMove);
    //设置显示文字
    void setText(const QString &text);
    //设置防区样式
    void setButtonStyle(const ButtonStyle &buttonStyle);
    //设置防区状态
    void setButtonStatus(const ButtonStatus &buttonStatus);

Q_SIGNALS:
    void clicked();
    void doubleClicked();
};

#endif //BUTTONDEFENCE_H
