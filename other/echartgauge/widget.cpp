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

#ifdef webkit
    webView = new QWebView;
    webView1 = new QWebView;
    webView2 = new QWebView;
    webView3 = new QWebView;
#elif webengine
    webView = new QWebEngineView;
    webView1 = new QWebEngineView;
    webView2 = new QWebEngineView;
    webView3 = new QWebEngineView;
#endif

    QUrl url("file:///" + qApp->applicationDirPath() + "/gauge.html");
#if (defined webkit) || (defined webengine)
    ui->gridLayout->addWidget(webView, 0, 0);
    ui->gridLayout->addWidget(webView1, 0, 1);
    ui->gridLayout->addWidget(webView2, 1, 0);
    ui->gridLayout->addWidget(webView3, 1, 1);

    webView->load(url);
    webView1->load(url);
    webView2->load(url);
    webView3->load(url);
#endif
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    QString js = QString("setGaugeValue(%1)").arg(value);
#ifdef webkit
    webView->page()->mainFrame()->evaluateJavaScript(js);
    webView1->page()->mainFrame()->evaluateJavaScript(js);
    webView2->page()->mainFrame()->evaluateJavaScript(js);
    webView3->page()->mainFrame()->evaluateJavaScript(js);
#elif webengine
    webView->page()->runJavaScript(js);
    webView1->page()->runJavaScript(js);
    webView2->page()->runJavaScript(js);
    webView3->page()->runJavaScript(js);
#endif
}
