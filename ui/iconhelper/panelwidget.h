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
    QScrollArea *scrollArea;        //滚动区域
    QWidget *scrollAreaContents;    //滚动区域载体
    QFrame *frame;                  //放置设备的框架,自动变宽变高
    QVBoxLayout *verticalLayout;    //设备面板总布局
    QGridLayout *gridLayout;        //设备表格布局

    int margin;                     //边距
    int space;                      //设备之间的间隔
    bool autoWidth;                 //宽度自动拉伸
    bool autoHeight;                //高度自动拉伸

    int columnCount;                //面板列数
    QList<QWidget *> widgets;       //设备面板对象集合

public:
    int getMargin()                 const;
    int getSpace()                  const;
    bool getAutoWidth()             const;
    bool getAutoHeight()            const;

    int getColumnCount()            const;
    QList<QWidget *> getWidgets()   const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    //设置边距+间距
    void setMargin(int left, int top, int right, int bottom);
    void setMargin(int margin);
    void setSpace(int space);

    //设置自动填充宽度+自动填充高度
    void setAutoWidth(bool autoWidth);
    void setAutoHeight(bool autoHeight);

    //设置列数+窗体集合
    void setColumnCount(int columnCount);
    void setWidgets(QList<QWidget *> widgets);

    //载入窗体集合+指定位置插入新窗体+移除指定的窗体+清空窗体
    void loadWidgets();
    void insertWidget(int index, QWidget *widget);
    void removeWidget(QWidget *widget);
    void clearWidgets();
};

#endif // PANELWIDGET_H
