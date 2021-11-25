#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    QStringList urls;
    urls << "https://hls01open.ys7.com/openlive/6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8";
    urls << "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
    urls << "http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4";
    urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
    ui->cboxUrl->addItems(urls);
    ui->cboxUrl->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        ui->btnOpen->setText("关闭");
        QString url = ui->cboxUrl->currentText().trimmed();
        ui->playWidget->setUrl(url);
        ui->playWidget->open();
    } else {
        ui->btnOpen->setText("打开");
        ui->playWidget->close();
    }
}
