#include "mpvwidget.h"
#include "mpvthread.h"
#include "qtimer.h"
#include "qdebug.h"

MpvWidget::MpvWidget(QWidget *parent) : QWidget(parent)
{
    thread = new MpvThread(this);
}

MpvWidget::~MpvWidget()
{
    close();
}

void MpvWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void MpvWidget::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();
    clear();

    thread->play();
    thread->start();
}

void MpvWidget::pause()
{
    thread->pause();
}

void MpvWidget::next()
{
    thread->next();
}

void MpvWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(3000);
    }

    QTimer::singleShot(5, this, SLOT(clear()));
}

void MpvWidget::restart()
{
    //qDebug() << TIMEMS << "restart video" << objectName();
    close();
    QTimer::singleShot(10, this, SLOT(open()));
}

void MpvWidget::clear()
{
    update();
}