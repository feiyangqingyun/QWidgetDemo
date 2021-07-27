#ifndef WIDGET_H
#define WIDGET_H

#include "framelesswidget.h"

namespace Ui {
class Widget;
}

class Widget : public FramelessWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

private slots:
    void initForm();
    void titleDblClick();
    void windowStateChange(bool max);

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
};

#endif // WIDGET_H
