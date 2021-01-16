#ifndef MINIBLINK_H
#define MINIBLINK_H

#include <QWidget>
#include "wke.h"

class miniblink : public QWidget
{
    Q_OBJECT
public:
    explicit miniblink(QWidget *parent = 0);

    //初始化资源
    static void init();
    //释放资源
    static void release();

protected:
    //设置浏览器控件自动适应大小
    void resizeEvent(QResizeEvent *);

private:
    //浏览器控件对象
    wkeWebView webView;

signals:
    //网页载入完成
    void loadFinished(bool ok);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);

public:
    //给回调用的函数
    void loadFinish(bool ok);
    void receiveData(const QString &type, const QVariant &data);

public slots:
    //加载网址或者本地文件
    void load(const QString &url, bool file = false);
    //加载html内容
    void setHtml(const QString &html, const QString &baseUrl);
    //执行js函数
    void runJs(const QString &js);
};

#endif // MINIBLINK_H
