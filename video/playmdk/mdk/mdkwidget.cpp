#include "mdkwidget.h"

MdkWidget::MdkWidget(QWidget *parent): OpenGLWidget(parent)
{
    player = new mdk::Player;
    player->setRenderCallback([this](void *) {
        QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
    });
}

MdkWidget::~MdkWidget()
{
    makeCurrent();
    mdk::Player::foreignGLContextDestroyed();
    doneCurrent();
}

void MdkWidget::setUrl(const QString &url)
{
    player->setMedia(url.toUtf8().constData());
}

void MdkWidget::play()
{
    player->set(mdk::State::Playing);
}

void MdkWidget::pause()
{
    player->set(mdk::State::Paused);
}

void MdkWidget::stop()
{
    player->set(mdk::State::Stopped);
}

void MdkWidget::initializeGL()
{

}

void MdkWidget::resizeGL(int w, int h)
{
    float ratio = 1;
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    ratio = devicePixelRatioF();
#endif
    player->setVideoSurfaceSize(w * ratio, h * ratio, this);
}

void MdkWidget::paintGL()
{
    player->renderVideo(this);
}
