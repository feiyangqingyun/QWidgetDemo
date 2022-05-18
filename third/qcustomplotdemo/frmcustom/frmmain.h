#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
class QAbstractButton;

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    static QList<QColor> colors;
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmMain *ui;

    //左侧导航栏图标+按钮集合
    QList<int> icons;
    QList<QAbstractButton *> btns;

private slots:
    void initForm();        //初始化界面数据
    void initWidget();      //初始化子窗体
    void initNav();         //初始化导航按钮
    void initIcon();        //初始化导航按钮图标
    void buttonClicked();   //导航按钮单击事件
};

#endif // FRMMAIN_H
