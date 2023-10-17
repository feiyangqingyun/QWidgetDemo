#pragma execution_character_set("utf-8")

#include "frmbase64helper.h"
#include "ui_frmbase64helper.h"
#include "base64helper.h"
#include "qfiledialog.h"
#include "qdebug.h"

frmBase64Helper::frmBase64Helper(QWidget *parent) : QWidget(parent), ui(new Ui::frmBase64Helper)
{
    ui->setupUi(this);
}

frmBase64Helper::~frmBase64Helper()
{
    delete ui;
}

void frmBase64Helper::showTime(qint64 size1, qint64 size2)
{
    //统计用时
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    double elapsed = (double)timer.nsecsElapsed() / 1000000;
#else
    double elapsed = (double)timer.elapsed();
#endif
    QString time = QString::number(elapsed, 'f', 3);
    ui->labInfo->setText(QString("用时: %1 毫秒  大小: %2 -> %3").arg(time).arg(size1).arg(size2));
}

void frmBase64Helper::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "", "图片(*.png *.bmp *.jpg)");
    if (!fileName.isEmpty()) {
        ui->txtFile->setText(fileName);
        QPixmap pix(fileName);
        pix = pix.scaled(ui->labImage->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->labImage->setPixmap(pix);
    }
}

void frmBase64Helper::on_btnClear_clicked()
{
    ui->txtFile->clear();
    ui->txtText->clear();
    ui->txtBase64->clear();
    ui->labImage->clear();
}

void frmBase64Helper::on_btnImageToBase64_clicked()
{
    QString fileName = ui->txtFile->text().trimmed();
    if (fileName.isEmpty()) {
        return;
    }

    timer.restart();
    QImage image(fileName);
    QString text = Base64Helper::imageToBase64(image);
    showTime(QFile(fileName).size(), text.size());
    ui->txtBase64->setText(text);
}

void frmBase64Helper::on_btnBase64ToImage_clicked()
{
    QString fileName = ui->txtFile->text().trimmed();
    QString text = ui->txtBase64->toPlainText().trimmed();
    if (text.isEmpty()) {
        return;
    }

    timer.restart();
    QImage image = Base64Helper::base64ToImage(text);
    showTime(text.size(), QFile(fileName).size());
    QPixmap pix = QPixmap::fromImage(image);
    pix = pix.scaled(ui->labImage->size() - QSize(4, 4), Qt::KeepAspectRatio);
    ui->labImage->setPixmap(pix);
}

void frmBase64Helper::on_btnTextToBase64_clicked()
{
    QString text = ui->txtText->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    timer.restart();
    QString result = Base64Helper::textToBase64(text);
    showTime(text.size(), result.size());
    ui->txtBase64->setText(result);
}

void frmBase64Helper::on_btnBase64ToText_clicked()
{
    QString text = ui->txtBase64->toPlainText().trimmed();
    if (text.isEmpty()) {
        return;
    }

    timer.restart();
    QString result = Base64Helper::base64ToText(text);
    showTime(text.size(), result.size());
    ui->txtText->setText(result);
}
