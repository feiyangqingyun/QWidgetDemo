#ifndef VLCWIDGET_H
#define VLCWIDGET_H

#include <QWidget>
class VlcThread;

class VlcWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VlcWidget(QWidget *parent = 0);
    ~VlcWidget();

private:
    VlcThread *thread;

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

#endif // VLCWIDGET_H
