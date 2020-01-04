#ifndef WIDGET_H
#define WIDGET_H

#ifdef webkit
#include <QtWebKit>
#else
#include <QtWebEngineWidgets>
#endif

namespace Ui
{
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void initForm();
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::Widget *ui;
#ifdef webkit
    QWebView *webView, *webView1, *webView2, *webView3;
#else
    QWebEngineView *webView, *webView1, *webView2, *webView3;
#endif

};

#endif // WIDGET_H
