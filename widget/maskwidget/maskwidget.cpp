#pragma execution_character_set("utf-8")

#include "maskwidget.h"
#include "qmutex.h"
#include "qapplication.h"
#include "qdebug.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include "qscreen.h"
#define deskGeometry qApp->primaryScreen()->geometry()
#define deskGeometry2 qApp->primaryScreen()->availableGeometry()
#else
#include "qdesktopwidget.h"
#define deskGeometry qApp->desktop()->geometry()
#define deskGeometry2 qApp->desktop()->availableGeometry()
#endif

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
    this->setGeometry(deskGeometry);
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
    if (mainWidget) {
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
        if (mainWidget) {
            if (obj->objectName() == mainWidget->objectName()) {
                if (this->isVisible()) {
                    this->activateWindow();
                }
            }
        }
    }

    return QObject::eventFilter(obj, event);
}
