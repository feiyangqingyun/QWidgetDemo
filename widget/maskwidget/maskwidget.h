#ifndef MASKWIDGET_H
#define MASKWIDGET_H

/**
 * 弹窗遮罩层控件 作者:feiyangqingyun(QQ:517216493) 2016-12-26
 * 1. 可设置需要遮罩的主窗体，自动跟随主窗体位置显示遮罩面积。
 * 2. 只需要将弹窗窗体的名称一开始传入队列即可，足够简单。
 * 3. 可设置透明度。
 * 4. 可设置遮罩层颜色。
 * 5. 不阻塞消息循坏。
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT MaskWidget : public QWidget
#else
class MaskWidget : public QWidget
#endif

{
    Q_OBJECT
public:
    static MaskWidget *Instance();
    explicit MaskWidget(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    static QScopedPointer<MaskWidget> self;

    //需要遮罩的主窗体
    QWidget *mainWidget;
    //需要弹窗的窗体对象名称集合链表
    QStringList dialogNames;

public Q_SLOTS:
    //设置需要遮罩的主窗体
    void setMainWidget(QWidget *mainWidget);
    //设置需要弹窗的窗体对象名称集合链表
    void setDialogNames(const QStringList &dialogNames);

    //设置遮罩颜色
    void setBgColor(const QColor &bgColor);
    //设置颜色透明度
    void setOpacity(double opacity);
};

#endif // MASKWIDGET_H
