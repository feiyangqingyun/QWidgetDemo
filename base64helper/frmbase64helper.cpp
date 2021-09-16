#pragma execution_character_set("utf-8")

#include "frmbase64helper.h"
#include "ui_frmbase64helper.h"
#include "base64helper.h"
#include "qfiledialog.h"
#include "qelapsedtimer.h"
#include "qdebug.h"

frmBase64Helper::frmBase64Helper(QWidget *parent) : QWidget(parent), ui(new Ui::frmBase64Helper)
{
    ui->setupUi(this);
}

frmBase64Helper::~frmBase64Helper()
{
    delete ui;
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
    //计时
    QElapsedTimer time;
    time.start();

    QString fileName = ui->txtFile->text().trimmed();
    if (!fileName.isEmpty()) {
        ui->txtBase64->setText(Base64Helper::imageToBase64(QImage(fileName)));
    }

    //统计用时
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    double elapsed = (double)time.nsecsElapsed() / 1000000;
#else
    double elapsed = (double)time.elapsed();
#endif
    QString strTime = QString::number(elapsed, 'f', 3);
    qDebug() << QString("用时 %1 毫秒").arg(strTime);
}

void frmBase64Helper::on_btnBase64ToImage_clicked()
{
    //计时
    QElapsedTimer time;
    time.start();

    QString text = ui->txtBase64->toPlainText().trimmed();
    if (!text.isEmpty()) {
        QPixmap pix = QPixmap::fromImage(Base64Helper::base64ToImage(text));
        pix = pix.scaled(ui->labImage->size() - QSize(4, 4), Qt::KeepAspectRatio);
        ui->labImage->setPixmap(pix);
    }

    //统计用时
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
    double elapsed = (double)time.nsecsElapsed() / 1000000;
#else
    double elapsed = (double)time.elapsed();
#endif
    QString strTime = QString::number(elapsed, 'f', 3);
    qDebug() << QString("用时 %1 毫秒").arg(strTime);
}

void frmBase64Helper::on_btnTextToBase64_clicked()
{
    QString text = ui->txtText->text().trimmed();
    if (!text.isEmpty()) {
        ui->txtBase64->setText(Base64Helper::textToBase64(text));
    }
}

void frmBase64Helper::on_btnBase64ToText_clicked()
{
    QString text = ui->txtBase64->toPlainText().trimmed();
    if (!text.isEmpty()) {
        ui->txtText->setText(Base64Helper::base64ToText(text));
    }
}
