#ifndef VLCTHREAD_H
#define VLCTHREAD_H

#include <QThread>
#include "vlcinclude.h"

class VlcThread : public QThread
{
    Q_OBJECT
public:
    explicit VlcThread(QObject *parent = 0);

protected:
    void run();

private:
    //线程停止标志位
    volatile bool stopped;
    //播放视频标志位
    volatile bool isPlay;

    //视频流地址
    QString url;
    //载体对象
    libvlc_instance_t *vlcInst;
    //媒体对象
    libvlc_media_t *vlcMedia;
    //播放对象
    libvlc_media_player_t *vlcPlayer;

public slots:
    //设置视频流地址
    void setUrl(const QString &url);
    void setOption(const QString &option);

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

#endif // VLCTHREAD_H
