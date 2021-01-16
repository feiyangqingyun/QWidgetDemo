#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

/**
 * 无边框窗体类 作者:feiyangqingyun(QQ:517216493) 2019-10-03
 * 1. 可以指定需要无边框的widget
 * 2. 边框四周八个方位都可以自由拉伸
 * 3. 可设置对应位置的边距,以便识别更大区域
 * 4. 可设置是否允许拖动
 * 5. 可设置是否允许拉伸
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT FramelessWidget : public QObject
#else
class FramelessWidget : public QObject
#endif

{
    Q_OBJECT
public:
    explicit FramelessWidget(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    int padding;                    //边距
    bool moveEnable;                //可移动
    bool resizeEnable;              //可拉伸
    QWidget *widget;                //无边框窗体

    bool pressed;                   //鼠标按下
    bool pressedLeft;               //鼠标按下左侧
    bool pressedRight;              //鼠标按下右侧
    bool pressedTop;                //鼠标按下上侧
    bool pressedBottom;             //鼠标按下下侧
    bool pressedLeftTop;            //鼠标按下左上侧
    bool pressedRightTop;           //鼠标按下右上侧
    bool pressedLeftBottom;         //鼠标按下左下侧
    bool pressedRightBottom;        //鼠标按下右下侧

    int rectX, rectY, rectW, rectH; //窗体坐标+宽高
    QPoint lastPos;                 //鼠标按下处坐标

    QRect rectLeft;                 //左侧区域
    QRect rectRight;                //右侧区域
    QRect rectTop;                  //上侧区域
    QRect rectBottom;               //下侧区域
    QRect rectLeftTop;              //左上侧区域
    QRect rectRightTop;             //右上侧区域
    QRect rectLeftBottom;           //左下侧区域
    QRect rectRightBottom;          //右下侧区域

public Q_SLOTS:
    //设置边距
    void setPadding(int padding);
    //设置是否可拖动+拉伸
    void setMoveEnable(bool moveEnable);
    void setResizeEnable(bool resizeEnable);

    //设置要无边框的窗体
    void setWidget(QWidget *widget);
};

#endif // FRAMELESSWIDGET_H
