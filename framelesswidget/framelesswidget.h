#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

/**
 * 无边框窗体类 作者:feiyangqingyun(QQ:517216493) 2019-10-03
 * 1. 可以指定需要无边框的widget。
 * 2. 边框四周八个方位都可以自由拉伸。
 * 3. 可设置对应位置的边距，以便识别更大区域。
 * 4. 可设置是否允许拖动。
 * 5. 可设置是否允许拉伸。
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
    //边距+可移动+可拉伸
    int padding;
    bool moveEnable;
    bool resizeEnable;

    //无边框窗体
    QWidget *widget;

    //鼠标是否按下+按下坐标+按下时窗体区域
    bool mousePressed;
    QPoint mousePoint;
    QRect mouseRect;

    //鼠标是否按下某个区域+按下区域的大小
    //依次为 左侧+右侧+上侧+下侧+左上侧+右上侧+左下侧+右下侧
    QList<bool> pressedArea;
    QList<QRect> pressedRect;

    //记录是否最小化
    bool isMin;
    //存储窗体默认的属性
    Qt::WindowFlags flags;

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
