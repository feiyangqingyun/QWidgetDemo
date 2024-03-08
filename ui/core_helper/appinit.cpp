#include "appinit.h"
#include "qmutex.h"
#include "qapplication.h"
#include "qevent.h"
#include "qwidget.h"
#include "qdebug.h"

QScopedPointer<AppInit> AppInit::self;
AppInit *AppInit::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppInit);
        }
    }

    return self.data();
}

AppInit::AppInit(QObject *parent) : QObject(parent)
{
}

bool AppInit::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *w = (QWidget *)watched;
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(watched, event);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    int type = event->type();
    QPoint p;
    QMouseEvent *mouseEvent = (QMouseEvent *)(event);    
    if (mouseEvent) {
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
        p = mouseEvent->globalPos();
#else
        p = mouseEvent->globalPosition().toPoint();
#endif
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = p - w->pos();
        }
    } else if (type == QEvent::MouseButtonRelease) {
        mousePressed = false;
    } else if (type == QEvent::MouseMove) {
        if (mousePressed) {
            w->move(p - mousePoint);
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void AppInit::start()
{
    qApp->installEventFilter(this);
}
