#include "ffmpegwidget.h"
#include "ffmpegthread.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qdebug.h"

FFmpegWidget::FFmpegWidget(QWidget *parent) : QWidget(parent)
{
    thread = new FFmpegThread(this);
    connect(thread, SIGNAL(receiveImage(QImage)), this, SLOT(updateImage(QImage)));
    image = QImage();
}

FFmpegWidget::~FFmpegWidget()
{
    close();
}

void FFmpegWidget::paintEvent(QPaintEvent *)
{
    if (image.isNull()) {
        return;
    }

    //qDebug() << TIMEMS << "paintEvent" << objectName();
    QPainter painter(this);
#if 0
    //image = image.scaled(this->size(), Qt::KeepAspectRatio);
    //按照比例自动居中绘制
    int pixX = rect().center().x() - image.width() / 2;
    int pixY = rect().center().y() - image.height() / 2;
    QPoint point(pixX, pixY);
    painter.drawImage(point, image);
#else
    painter.drawImage(this->rect(), image);
#endif
}

void FFmpegWidget::updateImage(const QImage &image)
{
    //this->image = image.copy();
    this->image = image;
    this->update();
}

void FFmpegWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void FFmpegWidget::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();
    clear();

    thread->play();
    thread->start();
}

void FFmpegWidget::pause()
{
    thread->pause();
}

void FFmpegWidget::next()
{
    thread->next();
}

void FFmpegWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(500);
    }

    QTimer::singleShot(1, this, SLOT(clear()));
}

void FFmpegWidget::clear()
{
    image = QImage();
    update();
}
