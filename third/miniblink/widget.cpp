#include "widget.h"
#include "ui_widget.h"
#include "qapplication.h"
#include "qdebug.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->initForm();
    on_btnLoadFile_clicked();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initForm()
{
    webView = new miniblink;
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(webView, SIGNAL(receiveDataFromJs(QString, QVariant)),
            this, SLOT(receiveDataFromJs(QString, QVariant)));
    ui->gridLayout->addWidget(webView, 0, 0);
}

void Widget::loadFinished(bool ok)
{
    qDebug() << "加载完成" << ok;
}

void Widget::receiveDataFromJs(const QString &type, const QVariant &data)
{
    qDebug() << "收到数据" << type << data;
}

void Widget::on_btnLoadUrl_clicked()
{
    webView->load("https://www.baidu.com");
}

void Widget::on_btnLoadFile_clicked()
{
    webView->load(qApp->applicationDirPath() + "/demo.html", true);
}

void Widget::on_btnLoadHtml_clicked()
{
    QStringList html;
    html << "<html><body>";
    html << "<h2>Hello World</h2>";
    html << "</body></html>";
    webView->setHtml(html.join(""), "");
}

void Widget::on_btnRunJs_clicked()
{
    webView->load(qApp->applicationDirPath() + "/gauge.html", true);
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    QString js = QString("setGaugeValue(%1)").arg(value);
    webView->runJs(js);
}
