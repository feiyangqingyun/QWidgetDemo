#ifndef FRAMELESSWIDGET2_H
#define FRAMELESSWIDGET2_H

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT FramelessWidget2 : public QWidget
#else
class FramelessWidget2 : public QWidget
#endif

{
    Q_OBJECT
public:
    explicit FramelessWidget2(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

    //拦截系统事件用于修复系统休眠后唤醒程序的BUG
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

public:
    //设置边距+可拖动+可拉伸
    void setPadding(int padding);
    void setMoveEnable(bool moveEnable);
    void setResizeEnable(bool resizeEnable);
};

#endif // FRAMELESSWIDGET2_H
