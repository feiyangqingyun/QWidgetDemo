#ifndef QUITIPBOX_H
#define QUITIPBOX_H

#include "quihead.h"

class QUITipBox : public QDialog
{
    Q_OBJECT

public:
    static QUITipBox *Instance();
    explicit QUITipBox(QWidget *parent = 0);
    ~QUITipBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUITipBox> self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout2;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labCountDown;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QLabel *labInfo;

    QPropertyAnimation *animation;
    bool fullScreen;

private:
    int closeSec;       //总显示时间
    int currentSec;     //当前已显示时间

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void checkSec();    //校验倒计时

private slots:
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &icon, quint32 size = 12);
    void setTip(const QString &title, const QString &tip, bool fullScreen = false, bool center = true, int closeSec = 0);
    void hide();
};

#endif // QUITIPBOX_H
