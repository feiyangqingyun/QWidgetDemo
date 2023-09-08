#ifndef PANELWIDGET_H
#define PANELWIDGET_H

/**
 * 面板容器控件 作者:feiyangqingyun(QQ:517216493) 2016-11-20
 * 1. 支持所有widget子类对象，自动产生滚动条。
 * 2. 支持自动拉伸自动填充。
 * 3. 提供接口获取容器内的所有对象的指针。
 * 4. 可设置是否自动拉伸宽度高度。
 * 5. 可设置设备面板之间的间距和边距。
 */

#include <QWidget>

class QScrollArea;
class QFrame;
class QVBoxLayout;
class QGridLayout;
class QSpacerItem;

#ifdef quc
class Q_DECL_EXPORT PanelWidget : public QWidget
#else
class PanelWidget : public QWidget
#endif

{
    Q_OBJECT

    Q_PROPERTY(int margin READ getMargin WRITE setMargin)
    Q_PROPERTY(int space READ getSpace WRITE setSpace)
    Q_PROPERTY(bool autoWidth READ getAutoWidth WRITE setAutoWidth)
    Q_PROPERTY(bool autoHeight READ getAutoHeight WRITE setAutoHeight)
    Q_PROPERTY(int columnCount READ getColumnCount WRITE setColumnCount)

public:
    explicit PanelWidget(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);

private:
    QScrollArea *scrollArea;    //滚动区域
    QWidget *scrollAreaContents;//滚动区域载体
    QFrame *frame;              //放置设备的框架,自动变宽变高
    QVBoxLayout *verticalLayout;//设备面板总布局
    QGridLayout *gridLayout;    //设备表格布局
    QSpacerItem *hSpacer;       //横向弹簧
    QSpacerItem *vSpacer;       //垂直弹簧

    int margin;                 //边距
    int space;                  //设备之间的间隔
    bool autoWidth;             //宽度自动拉伸
    bool autoHeight;            //高度自动拉伸

    int columnCount;            //面板列数
    QList<QWidget *> widgets;   //设备面板对象集合

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //设置边距
    void setMargin(int left, int top, int right, int bottom);

    //获取和设置边距
    int getMargin() const;
    void setMargin(int margin);

    //获取和设置间距
    int getSpace() const;
    void setSpace(int space);

    //获取和设置自动填充宽度
    bool getAutoWidth() const;
    void setAutoWidth(bool autoWidth);

    //获取和设置自自动填充高度
    bool getAutoHeight() const;
    void setAutoHeight(bool autoHeight);

    //获取和设置列数
    int getColumnCount() const;
    void setColumnCount(int columnCount);

    //获取和设置窗体集合
    QList<QWidget *> getWidgets() const;
    void setWidgets(QList<QWidget *> widgets);

    //载入窗体集合
    void loadWidgets();
    //指定位置插入新窗体
    void insertWidget(int index, QWidget *widget);
    //移除指定的窗体
    void removeWidget(QWidget *widget);
    //清空窗体
    void clearWidgets();
};

#endif // PANELWIDGET_H
