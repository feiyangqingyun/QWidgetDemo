#ifndef VLC_H
#define VLC_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#include "mpvhead.h"

class MpvThread : public QThread
{
    Q_OBJECT
public:
    explicit MpvThread(QObject *parent = 0);

protected:
    void run();

private:
    volatile bool stopped;              //线程停止标志位
    volatile bool isPlay;               //播放视频标志位

    QString url;                        //视频流地址
    mpv_handle *mpvPlayer;              //载体对象

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

//实时视频显示窗体类
class MpvWidget : public QWidget
{
    Q_OBJECT    
public:
    explicit MpvWidget(QWidget *parent = 0);
    ~MpvWidget();

private:
    MpvThread *thread;

public slots:    
    //设置视频流地址
    void setUrl(const QString &url);    

    //打开设备
    void open();
    //暂停
    void pause();
    //继续
    void next();
    //关闭设备
    void close();
    //重新加载
    void restart();
    //清空
    void clear();

};

#endif // VLC_H
