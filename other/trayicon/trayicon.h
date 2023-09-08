#ifndef TRAYICON_H
#define TRAYICON_H

/**
 * 托盘图标控件 作者:feiyangqingyun(QQ:517216493) 2017-01-08
 * 1. 可设置托盘图标对应所属主窗体。
 * 2. 可设置托盘图标。
 * 3. 可设置提示信息。
 * 4. 自带右键菜单。
 */

#include <QObject>
#include <QSystemTrayIcon>

class QMenu;

#ifdef quc
class Q_DECL_EXPORT TrayIcon : public QObject
#else
class TrayIcon : public QObject
#endif

{
    Q_OBJECT
public:
    static TrayIcon *Instance();
    explicit TrayIcon(QObject *parent = 0);

private:
    static QScopedPointer<TrayIcon> self;
    QWidget *mainWidget;            //对应所属主窗体
    QSystemTrayIcon *trayIcon;      //托盘对象
    QMenu *menu;                    //右键菜单
    bool exitDirect;                //是否直接退出

private slots:
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);

public:
    //设置是否直接退出,如果不是直接退出则发送信号给主界面
    void setExitDirect(bool exitDirect);

    //设置所属主窗体
    void setMainWidget(QWidget *mainWidget);    

    //显示消息
    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 5000);

    //设置图标
    void setIcon(const QString &strIcon);
    //设置提示信息
    void setToolTip(const QString &tip);

    //获取和设置是否可见
    bool getVisible() const;
    void setVisible(bool visible);

public Q_SLOTS:
    //退出所有
    void closeAll();
    //显示主窗体
    void showMainWidget();

Q_SIGNALS:
    void trayIconExit();
};

#endif // TRAYICON_H
