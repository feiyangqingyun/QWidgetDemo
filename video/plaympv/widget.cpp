#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"
#include "qfiledialog.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    QStringList urls;
    urls << "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
    urls << "http://vd3.bdstatic.com/mda-jennyc5ci1ugrxzi/mda-jennyc5ci1ugrxzi.mp4";
    urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
    ui->cboxUrl->addItems(urls);
    ui->cboxUrl->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName();
    if (!fileName.isEmpty()) {
        ui->cboxUrl->addItem(fileName);
        ui->cboxUrl->lineEdit()->setText(fileName);
        if (ui->btnOpen->text() == "打开") {
            on_btnOpen_clicked();
        }
    }
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
