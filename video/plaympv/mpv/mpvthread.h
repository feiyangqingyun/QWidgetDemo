#ifndef MPVTHREAD_H
#define MPVTHREAD_H

#include <QThread>
#include <QVariant>
#include "mpvinclude.h"

class MpvThread : public QThread
{
    Q_OBJECT
public:
    explicit MpvThread(QObject *parent = 0);

protected:
    void run();

private:
    //线程停止标志位
    volatile bool stopped;
    //播放视频标志位
    volatile bool isPlay;

    //视频流地址
    QString url;
    //播放句柄
    mpv_handle *mpvPlayer;

signals:
    //收到图片信号
    void receiveImage(const QImage &image);

public slots:
    //设置视频流地址
    void setUrl(const QString &url);

    //通用属性接口
    QVariant getValue(const QString &name);
    int setValue(const QString &name, const QVariant &value);
    int setOption(const QString &name, const QVariant &value);
    QVariant command(const QVariant &args);

    //初始化视频对象
    bool init();
    //播放视频对象
    void play();
    //暂停播放
    void pause();
    //继续播放
    void next();
    //释放对象
    void free();
    //停止采集线程
    void stop();
};

#endif // MPVTHREAD_H
