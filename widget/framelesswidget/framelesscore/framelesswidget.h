#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

/**
 * 无边框窗体类 作者:feiyangqingyun(QQ:517216493) 2021-07-27
 * 1. 同时支持Qt4-Qt6，亲测Qt4.7到Qt6.2。
 * 2. 同时支持mingw、msvc、gcc等。
 * 3. 同时支持windows、linux、mac。
 * 4. 同时支持QMainWindow、QWidget、QDialog。
 * 5. 使用方法极其简单，只需要将继承类修改即可。
 * 6. 自动识别双击标题栏响应。
 * 7. 无边框拉伸在windows下不抖动。
 * 8. 在windows下具有移动到边缘半屏、移动到顶部全屏特性。
 * 9. 解决mac系统上无边框最小化最大化失效的bug。
 * 10. 解决系统休眠后再次启动程序懵逼的bug。
 * 11. 解决有时候窗体重新显示的时候假死不刷新的bug。
 * 12. 轻量级，1个代码文件，核心代码行数不到300行。
 * 13. 注释详细，示例完美，非常适合阅读和学习。
 * 14. 开源开箱即用，保证任意Qt版本可正常编译运行，无需任何调整。
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT FramelessWidget : public QWidget
#else
class FramelessWidget : public QWidget
#endif

{
    Q_OBJECT
public:
    explicit FramelessWidget(QWidget *parent = 0);

protected:
    //窗体显示的时候触发
    void showEvent(QShowEvent *event);

    //事件过滤器识别拖动拉伸等
    void doWindowStateChange(QEvent *event);
    void doResizeEvent(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

    //拦截系统事件用于修复系统休眠后唤醒程序的bug
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result);
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif

    //Qt4的写法
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif
#endif

private:
    //边距+可移动+可拉伸
    int padding;
    bool moveEnable;
    bool resizeEnable;

    //标题栏控件
    QWidget *titleBar;

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
    //设置边距+可拖动+可拉伸
    void setPadding(int padding);
    void setMoveEnable(bool moveEnable);
    void setResizeEnable(bool resizeEnable);

    //设置标题栏控件
    void setTitleBar(QWidget *titleBar);

Q_SIGNALS:
    void titleDblClick();
    void windowStateChange(bool max);
};

#endif // FRAMELESSWIDGET_H
