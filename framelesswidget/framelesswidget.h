#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

/**
 * 无边框窗体类 作者:feiyangqingyun(QQ:517216493) 2019-10-03
 * 1:可以指定需要无边框的widget
 * 2:边框四周八个方位都可以自由拉伸
 * 3:可设置对应位置的边距,以便识别更大区域
 */

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT FramelessWidget : public QObject
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

    QRectF rectLeft;                //左侧区域
    QRectF rectRight;               //右侧区域
    QRectF rectTop;                 //上侧区域
    QRectF rectBottom;              //下侧区域
    QRectF rectLeftTop;             //左上侧区域
    QRectF rectRightTop;            //右上侧区域
    QRectF rectLeftBottom;          //左下侧区域
    QRectF rectRightBottom;         //右下侧区域

public Q_SLOTS:
    //设置边距
    void setPadding(int padding);
    //设置要无边框的窗体
    void setWidget(QWidget *widget);
};

#endif // FRAMELESSWIDGET_H
