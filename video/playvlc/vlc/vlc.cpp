#include "vlc.h"

VlcThread::VlcThread(QObject *parent) : QThread(parent)
{
    setObjectName("VlcThread");

    stopped = false;
    isPlay = false;

    url = "rtsp://192.168.1.200:554/1";

    vlcInst = NULL;
    vlcMedia = NULL;
    vlcPlayer = NULL;

    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        qDebug() << TIMEMS << "init vlc lib ok" << " version:" << libvlc_get_version();
    }
}

void VlcThread::run()
{
    while (!stopped) {
        msleep(1);
    }

    //线程结束后释放资源
    free();
    stopped = false;
    isPlay = false;
    //qDebug() << TIMEMS << "stop vlc1 thread";
}

void VlcThread::setUrl(const QString &url)
{
    this->url = url;
}

void VlcThread::setOption(const QString &option)
{
    if (vlcMedia != NULL) {
        QByteArray data = option.toUtf8();
        const char *arg = data.constData();
        libvlc_media_add_option(vlcMedia, arg);
    }
}

bool VlcThread::init()
{
    const char *tempArg = "";
    const char *vlc_args[9] = {"-I", "dummy", "--no-osd", "--no-stats", "--ignore-config", "--no-video-on-top", "--no-video-title-show", "--no-snapshot-preview", tempArg};
    vlcInst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (vlcInst == NULL) {
        return false;
    }

    vlcMedia = libvlc_media_new_location(vlcInst, url.toUtf8().constData());
    vlcPlayer = libvlc_media_player_new_from_media(vlcMedia);
    if (vlcPlayer == NULL) {
        return false;
    }

    //设置播放句柄
    VlcWidget *w = (VlcWidget *)this->parent();
#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(vlcPlayer, (void *)w->winId());
#elif defined(Q_OS_LINUX)
    libvlc_media_player_set_xwindow(vlcPlayer, w->winId());
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(vlcPlayer, (void *)w->winId());
#endif

    //设置硬件加速 none auto any d3d11va dxva2
    setOption(QString(":avcodec-hw=%1").arg("none"));
    //设置通信协议 tcp udp
    setOption(QString(":rtsp-%1").arg("tcp"));
    //设置缓存时间 默认500毫秒
    setOption(QString(":network-caching=%1").arg(300));

    libvlc_media_player_play(vlcPlayer);
    //qDebug() << TIMEMS << "init vlc finsh";
    return true;
}

void VlcThread::play()
{
    isPlay = true;
    this->init();
}

void VlcThread::pause()
{
    if (vlcPlayer != NULL) {
        libvlc_media_player_pause(vlcPlayer);
    }
}

void VlcThread::next()
{
    if (vlcPlayer != NULL) {
        libvlc_media_player_pause(vlcPlayer);
    }
}

void VlcThread::free()
{
    if (vlcInst != NULL) {
        libvlc_release(vlcInst);
        vlcInst = NULL;
    }

    if (vlcMedia != NULL) {
        libvlc_media_release(vlcMedia);
        vlcMedia = NULL;
    }

    if (vlcPlayer != NULL) {
        libvlc_media_player_release(vlcPlayer);
        vlcPlayer = NULL;
    }

    //qDebug() << TIMEMS << "close vlc ok";
}

void VlcThread::stop()
{
    stopped = true;
}

//实时视频显示窗体类
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
