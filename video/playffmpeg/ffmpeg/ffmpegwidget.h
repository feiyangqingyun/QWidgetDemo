#ifndef FFMPEGWIDGET_H
#define FFMPEGWIDGET_H

#include <QWidget>
#include <QImage>
class FFmpegThread;

class FFmpegWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFmpegWidget(QWidget *parent = 0);
    ~FFmpegWidget();

protected:
    void paintEvent(QPaintEvent *);

private:
    //解码图片
    QImage image;
    //解码线程
    FFmpegThread *thread;

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

#endif // FFMPEGWIDGET_H
