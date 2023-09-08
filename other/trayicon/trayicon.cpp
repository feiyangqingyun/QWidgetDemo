#pragma execution_character_set("utf-8")

#include "trayicon.h"
#include "qmutex.h"
#include "qmenu.h"
#include "qapplication.h"
#include "qdebug.h"

QScopedPointer<TrayIcon> TrayIcon::self;
TrayIcon *TrayIcon::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new TrayIcon);
        }
    }

    return self.data();
}

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    mainWidget = 0;
    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    menu = new QMenu;
    exitDirect = true;
}

void TrayIcon::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick: {
            this->showMainWidget();
            break;
        }

        default:
            break;
    }
}

void TrayIcon::setExitDirect(bool exitDirect)
{
    if (this->exitDirect != exitDirect) {
        this->exitDirect = exitDirect;
    }
}

void TrayIcon::setMainWidget(QWidget *mainWidget)
{
    this->mainWidget = mainWidget;
    menu->addAction("主界面", this, SLOT(showMainWidget()));

    if (exitDirect) {
        menu->addAction("退出", this, SLOT(closeAll()));
    } else {
        menu->addAction("退出", this, SIGNAL(trayIconExit()));
    }

    trayIcon->setContextMenu(menu);
}

void TrayIcon::showMainWidget()
{
    if (mainWidget) {
        mainWidget->showNormal();
        mainWidget->activateWindow();
    }
}

void TrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msecs)
{
    trayIcon->showMessage(title, msg, icon, msecs);
}

void TrayIcon::setIcon(const QString &strIcon)
{
    trayIcon->setIcon(QIcon(strIcon));
}

void TrayIcon::setToolTip(const QString &tip)
{
    trayIcon->setToolTip(tip);
}

bool TrayIcon::getVisible() const
{
    return trayIcon->isVisible();
}

void TrayIcon::setVisible(bool visible)
{
    trayIcon->setVisible(visible);
}

void TrayIcon::closeAll()
{
    trayIcon->hide();
    trayIcon->deleteLater();
    qApp->exit();
}
