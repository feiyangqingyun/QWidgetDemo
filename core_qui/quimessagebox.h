#ifndef QUIMESSAGEBOX_H
#define QUIMESSAGEBOX_H

#include "quihead.h"

class QUIMessageBox : public QDialog
{
    Q_OBJECT

public:
    static QUIMessageBox *Instance();
    explicit QUIMessageBox(QWidget *parent = 0);
    ~QUIMessageBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIMessageBox> self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout3;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labCountDown;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout4;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout4;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIcoMain;
    QSpacerItem *horizontalSpacer1;
    QLabel *labInfo;
    QHBoxLayout *horizontalLayout2;
    QSpacerItem *horizontalSpacer2;
    QPushButton *btnOk;
    QPushButton *btnCancel;

private:
    int closeSec;       //总显示时间
    int currentSec;     //当前已显示时间

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void checkSec();    //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &icon, quint32 size = 12);
    void setIconMsg(const QString &png, const QChar &str);
    void setMessage(const QString &msg, int type, int closeSec = 0);
};

#endif // QUIMESSAGEBOX_H
