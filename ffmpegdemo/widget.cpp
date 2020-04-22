#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QStringList urls;
    urls << "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
    urls << "rtsp://admin:Admin123456@192.168.1.64:554/Streaming/Channels/102?transportmode=unicast&profile=Profile_2";
    urls << "rtsp://192.168.1.108:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif";
    urls << "rtsp://192.168.1.15:554/media/video1";
    urls << "rtsp://192.168.1.15:554/media/video2";
    urls << "rtsp://192.168.1.247:554/av0_0";
    urls << "rtsp://192.168.1.247:554/av0_1";
    ui->cboxUrl->addItems(urls);
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
        ui->ffmpegWidget->setUrl(url);
        ui->ffmpegWidget->open();
    } else {
        ui->btnOpen->setText("打开");
        ui->ffmpegWidget->close();
    }
}
