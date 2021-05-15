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
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif

private:
    int padding;
};

#endif // FRAMELESSWIDGET2_H
