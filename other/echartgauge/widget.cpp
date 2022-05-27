#include "widget.h"
#include "ui_widget.h"
#include "qurl.h"

#ifdef webkit
#include <QtWebKit>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWebKitWidgets>
#endif
#elif webengine
#include <QtWebEngineWidgets>
#endif

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->initForm();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initForm()
{
    //初始化浏览器控件属性,一个项目中只需要设置一次就行
#ifdef webkit
    QWebSettings *webSetting = QWebSettings::globalSettings();
    webSetting->setAttribute(QWebSettings::JavascriptEnabled, true);
    webSetting->setAttribute(QWebSettings::PluginsEnabled, true);
    webSetting->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
#elif webengine
    QWebEngineSettings *webSetting = QWebEngineProfile::defaultProfile()->settings();
    webSetting->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    webSetting->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    webSetting->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
#endif

    //实例化多个浏览器控件,设置背景透明
#ifdef webkit
    for (int i = 0; i < 4; ++i) {
        QWebView *webView = new QWebView;
        webView->setStyleSheet(QString("background:%1;").arg("rgba(255,0,0,0)"));
        webViews << webView;
    }
#elif webengine
    for (int i = 0; i < 4; ++i) {
        QWebEngineView *webView = new QWebEngineView;
        webView->page()->setBackgroundColor(Qt::transparent);
        webViews << webView;
    }
#endif

    //添加到布局
#if (defined webkit) || (defined webengine)
    ui->gridLayout->addWidget(webViews.at(0), 0, 0);
    ui->gridLayout->addWidget(webViews.at(1), 0, 1);
    ui->gridLayout->addWidget(webViews.at(2), 1, 0);
    ui->gridLayout->addWidget(webViews.at(3), 1, 1);
#endif

    //加载html文件
    QUrl url("file:///" + qApp->applicationDirPath() + "/gauge.html");
#ifdef webkit
    foreach (QWebView *webView, webViews) {
        webView->load(url);
    }
#elif webengine
    foreach (QWebEngineView *webView, webViews) {
        webView->load(url);
    }
#endif
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    //执行js函数
    QString js = QString("setGaugeValue(%1)").arg(value);
#ifdef webkit
    foreach (QWebView *webView, webViews) {
        webView->page()->mainFrame()->evaluateJavaScript(js);
    }
#elif webengine
    foreach (QWebEngineView *webView, webViews) {
        webView->page()->runJavaScript(js);
    }
#endif
}
