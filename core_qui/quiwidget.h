#ifndef QUIWIDGET_H
#define QUIWIDGET_H

#include "quihead.h"

class QUIWidget : public QDialog
{
    Q_OBJECT

public:
    //将部分对象作为枚举值暴露给外部
    enum Widget {
        Lab_Ico = 0,            //左上角图标
        BtnMenu = 1,            //下拉菜单按钮
        BtnMenu_Min = 2,        //最小化按钮
        BtnMenu_Max = 3,        //最大化按钮
        BtnMenu_Normal = 4,     //还原按钮
        BtnMenu_Close = 5       //关闭按钮
    };

    explicit QUIWidget(QWidget *parent = 0);
    ~QUIWidget();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QVBoxLayout *verticalLayout1;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout4;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QToolButton *btnMenu;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Max;
    QPushButton *btnMenu_Close;
    QWidget *widget;
    QVBoxLayout *verticalLayout3;

private:
    QString title;          //标题
    Qt::Alignment alignment;//标题文本对齐
    bool minHide;           //最小化隐藏
    bool exitAll;           //退出整个程序
    QWidget *mainWidget;    //主窗体对象

public:
    QLabel *getLabIco()             const;
    QLabel *getLabTitle()           const;

    QToolButton *getBtnMenu()       const;
    QPushButton *getBtnMenuMin()    const;
    QPushButton *getBtnMenuMax()    const;
    QPushButton *getBtnMenuClose() const;

    QString getTitle()              const;
    Qt::Alignment getAlignment()    const;
    bool getMinHide()               const;
    bool getExitAll()               const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void changeStyle(); //更换样式

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    //设置部件图标
    void setIcon(QUIWidget::Widget widget, int icon, quint32 size = 12);
    void setIconMain(int icon, quint32 size = 12);
    //设置部件图片
    void setPixmap(QUIWidget::Widget widget, const QString &file, const QSize &size = QSize(16, 16));
    //设置部件是否可见
    void setVisible(QUIWidget::Widget widget, bool visible = true);
    //设置只有关闭按钮
    void setOnlyCloseBtn();

    //设置标题栏高度
    void setTitleHeight(int height);
    //设置按钮统一宽度
    void setBtnWidth(int width);

    //设置标题及文本样式
    void setTitle(const QString &title);
    void setAlignment(Qt::Alignment alignment);

    //设置最小化隐藏
    void setMinHide(bool minHide);

    //设置退出时候直接退出整个应用程序
    void setExitAll(bool exitAll);

    //设置主窗体
    void setMainWidget(QWidget *mainWidget);

Q_SIGNALS:
    void changeStyle(const QString &qssFile);
    void closing();
};

#endif // QUIWIDGET_H
