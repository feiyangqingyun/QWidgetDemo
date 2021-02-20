#ifndef FRMBASE64_H
#define FRMBASE64_H

#include <QWidget>

namespace Ui {
class frmBase64;
}

class frmBase64 : public QWidget
{
    Q_OBJECT

public:
    explicit frmBase64(QWidget *parent = 0);
    ~frmBase64();

private:
    Ui::frmBase64 *ui;

private slots:
    //图片转base64编码
    QString getImageData(const QImage &image);
    QByteArray getImageData2(const QImage &image);
    //base64编码数据转图片
    QImage getImage(const QString &data);

    //汉字转base64编码
    QString getBase64(const QString &data);
    //base64编码转汉字
    QString getData(const QString &base64);

private slots:
    void on_btnOpen_clicked();
    void on_btnClear_clicked();
    void on_btnImageToBase64_clicked();
    void on_btnBase64ToImage_clicked();
    void on_btnTextToBase64_clicked();
    void on_btnBase64ToText_clicked();
};

#endif // FRMBASE64_H
