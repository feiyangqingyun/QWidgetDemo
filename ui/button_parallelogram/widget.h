#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void showEvent(QShowEvent *event) override;  // 声明 showEvent 函数

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
