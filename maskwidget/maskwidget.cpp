#pragma execution_character_set("utf-8")

#include "maskwidget.h"
#include "qmutex.h"
#include "qdesktopwidget.h"
#include "qapplication.h"
#include "qdebug.h"

QScopedPointer<MaskWidget> MaskWidget::self;
MaskWidget *MaskWidget::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new MaskWidget);
        }
    }

    return self.data();
}

MaskWidget::MaskWidget(QWidget *parent) : QWidget(parent)
{
    mainWidget = 0;
    setOpacity(0.7);
    setBgColor(QColor(0, 0, 0));

    //不设置主窗体则遮罩层大小为默认桌面大小
    this->setGeometry(qApp->desktop()->geometry());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    //绑定全局事件,过滤弹窗窗体进行处理
    qApp->installEventFilter(this);
}

void MaskWidget::setMainWidget(QWidget *mainWidget)
{
    if (this->mainWidget != mainWidget) {
        this->mainWidget = mainWidget;
    }
}

void MaskWidget::setDialogNames(const QStringList &dialogNames)
{
    if (this->dialogNames != dialogNames) {
        this->dialogNames = dialogNames;
    }
}

void MaskWidget::setOpacity(double opacity)
{
    this->setWindowOpacity(opacity);
}

void MaskWidget::setBgColor(const QColor &bgColor)
{
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, bgColor);
    this->setPalette(palette);
}

void MaskWidget::showEvent(QShowEvent *)
{
    if (mainWidget != 0) {
        this->setGeometry(mainWidget->geometry());
    }
}

bool MaskWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show) {
        if (dialogNames.contains(obj->objectName())) {
            this->show();
            this->activateWindow();
            QWidget *w = (QWidget *)obj;
            w->activateWindow();
        }
    } else if (event->type() == QEvent::Hide) {
        if (dialogNames.contains(obj->objectName())) {
            this->hide();
        }
    } else if (event->type() == QEvent::WindowActivate) {
        //当主窗体激活时,同时激活遮罩层
        if (mainWidget != 0) {
            if (obj->objectName() == mainWidget->objectName()) {
                if (this->isVisible()) {
                    this->activateWindow();
                }
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
