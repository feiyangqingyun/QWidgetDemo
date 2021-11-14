#ifndef COLORWIDGET_H
#define COLORWIDGET_H

/**
 * 屏幕拾色器 作者:feiyangqingyun(QQ:517216493) 2016-11-11
 * 1. 鼠标按下实时采集鼠标处的颜色。
 * 2. 实时显示颜色值。
 * 3. 支持16进制格式和rgb格式。
 * 4. 实时显示预览颜色。
 * 5. 根据背景色自动计算合适的前景色。
 */

#include <QWidget>

class QGridLayout;
class QVBoxLayout;
class QLabel;
class QLineEdit;

#ifdef quc
class Q_DECL_EXPORT ColorWidget : public QWidget
#else
class ColorWidget : public QWidget
#endif

{
    Q_OBJECT
public:
    static ColorWidget *Instance();
    explicit ColorWidget(QWidget *parent = 0);
    ~ColorWidget();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    static ColorWidget *instance;
    QClipboard *cp;
    bool pressed;
    QTimer *timer;

    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labColor;
    QLabel *label;
    QLabel *labWeb;
    QLineEdit *txtWeb;
    QLabel *labRgb;
    QLineEdit *txtRgb;
    QLabel *labPoint;
    QLineEdit *txtPoint;

private Q_SLOTS:
    void showColorValue();
};

#endif // COLORWIDGET_H
