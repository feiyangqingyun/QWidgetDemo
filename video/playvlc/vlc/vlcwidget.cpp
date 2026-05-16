#include "vlcwidget.h"
#include "vlcthread.h"
#include "qtimer.h"
#include "qdebug.h"

VlcWidget::VlcWidget(QWidget *parent) : QWidget(parent)
{
    thread = new VlcThread(this);
}

VlcWidget::~VlcWidget()
{
    close();
}

void VlcWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void VlcWidget::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();
    clear();

    thread->play();
    thread->start();
}

void VlcWidget::pause()
{
    thread->pause();
}

void VlcWidget::next()
{
    thread->next();
}

void VlcWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(3000);
    }

    QTimer::singleShot(5, this, SLOT(clear()));
}

void VlcWidget::restart()
{
    //qDebug() << TIMEMS << "restart video" << objectName();
    close();
    QTimer::singleShot(10, this, SLOT(open()));
}

void VlcWidget::clear()
{
    update();
}
