#ifndef FFMPEG_H
#define FFMPEG_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#include "ffmpeginclude.h"

class FFmpegThread : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegThread(QObject *parent = 0);
    static void initlib();

protected:
    void run();

private:
    volatile bool stopped;          //线程停止标志位
    volatile bool isPlay;           //播放视频标志位

    int frameFinish;                //一帧完成
    int videoWidth;                 //视频宽度
    int videoHeight;                //视频高度
    int videoStreamIndex;           //视频流索引
    int audioStreamIndex;           //音频流索引

    QString url;                    //视频流地址

    uint8_t *buffer;                //存储解码后图片buffer
    AVPacket *avPacket;             //包对象
    AVFrame *yuvFrame;              //原始视频帧
    AVFrame *rgbFrame;              //转图片视频帧

    AVFormatContext *formatCtx;     //格式上下文
    AVCodecContext *videoCodecCtx;  //视频解码器上下文
    AVCodecContext *audioCodecCtx;  //音频解码器上下文
    SwsContext *swsContext;         //处理图片数据对象

    AVDictionary *options;          //参数对象
    AVCodec *videoCodec;            //视频解码器
    AVCodec *audioCodec;            //音频解码器

signals:
    //收到图片信号
    void receiveImage(const QImage &image);

public slots:
    //设置视频流地址
    void setUrl(const QString &url);

    //初始化视频对象
    bool init();
    //释放对象
    void free();
    //播放视频对象
    void play();
    //暂停播放
    void pause();
    //继续播放
    void next();
    //停止采集线程
    void stop();
};

//实时视频显示窗体类
class FFmpegWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFmpegWidget(QWidget *parent = 0);
    ~FFmpegWidget();

protected:   
    void paintEvent(QPaintEvent *);

private:
    FFmpegThread *thread;
    QImage image;

private slots:
    //接收图像并绘制
    void updateImage(const QImage &image);

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
    //清空
    void clear();
};

#endif // FFMPEG_H
