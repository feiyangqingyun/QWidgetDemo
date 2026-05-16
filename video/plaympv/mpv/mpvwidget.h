#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QWidget>
class MpvThread;

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

#endif // MPVWIDGET_H
