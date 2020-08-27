#include "mpv.h"
#include "mpvtool.h"

MpvThread::MpvThread(QObject *parent) : QThread(parent)
{
    setObjectName("MpvThread");

    stopped = false;
    isPlay = false;

    url = "rtsp://192.168.1.200:554/1";

    mpvPlayer = NULL;

    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        qDebug() << TIMEMS << "init mpv lib ok" << " version:" << MPV_CLIENT_API_VERSION;
    }
}

void MpvThread::run()
{
    while (!stopped) {
        msleep(1);
    }

    //线程结束后释放资源
    free();
    stopped = false;
    isPlay = false;
    //qDebug() << TIMEMS << "stop mpv thread";
}

void MpvThread::setUrl(const QString &url)
{
    this->url = url;
}

QVariant MpvThread::getValue(const QString &name)
{
    if (mpvPlayer != NULL) {
        return qtmpv::get_property(mpvPlayer, name);
    } else {
        return QVariant();
    }
}

int MpvThread::setValue(const QString &name, const QVariant &value)
{
    if (mpvPlayer != NULL) {
        return qtmpv::set_property(mpvPlayer, name, value);
    }

    return -1;
}

int MpvThread::setOption(const QString &name, const QVariant &value)
{
    if (mpvPlayer != NULL) {
        return qtmpv::set_option_variant(mpvPlayer, name, value);
    }

    return -1;
}

QVariant MpvThread::command(const QVariant &args)
{
    if (mpvPlayer != NULL) {
        return qtmpv::command_variant(mpvPlayer, args);
    }

    return QVariant();
}

bool MpvThread::init()
{
    //创建实例
    if (mpvPlayer == NULL) {
        mpvPlayer = mpv_create();
    }

    MpvWidget *widget = (MpvWidget *)this->parent();
    HWND wid = (HWND)widget->winId();
    mpv_set_option(mpvPlayer, "wid", MPV_FORMAT_INT64, &wid);

    //请求级别日志消息
    mpv_request_log_messages(mpvPlayer, "info");
    //启用默认绑定
    setValue("input-default-bindings", "yes");
    //启用键盘输入
    setValue("input-vo-keyboard", "yes");

    //设置控制台打印
    setOption("terminal", "false");
    //设置消息级别
    setOption("msg-level", "all=v");

    //设置硬件加速 none auto any d3d11va dxva2
    setOption("hwdec", "d3d11va");
    //设置通信协议 tcp udp
    setOption("rtsp-transport", "tcp");
    //设置网络超时时间 单位秒
    setOption("network-timeout", 3);

    //初始化实例
    if (mpv_initialize(mpvPlayer) < 0) {
        return false;
    }

    QByteArray data = url.toUtf8();
    const char *args[] = {"loadfile", data.data(), NULL};
    if (mpv_command_async(mpvPlayer, 0, args) < 0) {
        return false;
    }

    //qDebug() << TIMEMS << "init mpv finsh";
    return true;
}

void MpvThread::play()
{
    isPlay = true;
    this->init();
}

void MpvThread::pause()
{
    setValue("pause", "yes");
}

void MpvThread::next()
{
    setValue("pause", "no");
}

void MpvThread::free()
{
    if (mpvPlayer != NULL) {
        mpv_terminate_destroy(mpvPlayer);
        mpvPlayer = NULL;
    }

    //qDebug() << TIMEMS << "close mpv ok";
}

void MpvThread::stop()
{
    stopped = true;
}

//实时视频显示窗体类
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
