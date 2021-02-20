#pragma execution_character_set("utf-8")

#include "frmbase64.h"
#include "ui_frmbase64.h"
#include "qfiledialog.h"
#include "qbuffer.h"
#include "qdebug.h"

frmBase64::frmBase64(QWidget *parent) : QWidget(parent), ui(new Ui::frmBase64)
{
    ui->setupUi(this);
}

frmBase64::~frmBase64()
{
    delete ui;
}

QString frmBase64::getImageData(const QImage &image)
{
    return QString(getImageData2(image));
}

QByteArray frmBase64::getImageData2(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "JPG");
    imageData = imageData.toBase64();
    return imageData;
}

QImage frmBase64::getImage(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}

QString frmBase64::getBase64(const QString &data)
{
    return QString(data.toLocal8Bit().toBase64());
}

QString frmBase64::getData(const QString &base64)
{
    QByteArray data = QByteArray::fromBase64(base64.toLocal8Bit());
    return QString(data);
}

void frmBase64::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "", "图片(*.png *.bmp *.jpg)");
    if (!fileName.isEmpty()) {
        ui->txtFile->setText(fileName);
        QPixmap pix(fileName);
        pix = pix.scaled(ui->labImage->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->labImage->setPixmap(pix);
    }
}

void frmBase64::on_btnClear_clicked()
{
    ui->txtFile->clear();
    ui->txtText->clear();
    ui->txtBase64->clear();
    ui->labImage->clear();
}

void frmBase64::on_btnImageToBase64_clicked()
{
    QString fileName = ui->txtFile->text().trimmed();
    if (!fileName.isEmpty()) {
        ui->txtBase64->setText(getImageData(QImage(fileName)));
    }
}

void frmBase64::on_btnBase64ToImage_clicked()
{
    QString text = ui->txtBase64->toPlainText().trimmed();
    if (!text.isEmpty()) {
        QPixmap pix = QPixmap::fromImage(getImage(text));
        pix = pix.scaled(ui->labImage->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->labImage->setPixmap(pix);
    }
}

void frmBase64::on_btnTextToBase64_clicked()
{
    QString text = ui->txtText->text().trimmed();
    if (!text.isEmpty()) {
        ui->txtBase64->setText(getBase64(text));
    }
}

void frmBase64::on_btnBase64ToText_clicked()
{
    QString text = ui->txtBase64->toPlainText().trimmed();
    if (!text.isEmpty()) {
        ui->txtText->setText(getData(text));
    }
}
