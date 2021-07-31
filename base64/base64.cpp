#include "base64.h"
#include "qbuffer.h"
#include "qdebug.h"

QString Base64::imageToBase64(const QImage &image)
{
    return QString(imageToBase64x(image));
}

QByteArray Base64::imageToBase64x(const QImage &image)
{
    //这个转换可能比较耗时建议在线程中执行
    QByteArray data;
    QBuffer buffer(&data);
    image.save(&buffer, "JPG");
    data = data.toBase64();
    return data;
}

QImage Base64::base64ToImage(const QString &data)
{
    return base64ToImagex(data.toUtf8());
}

QImage Base64::base64ToImagex(const QByteArray &data)
{
    //这个转换可能比较耗时建议在线程中执行
    QImage image;
    image.loadFromData(QByteArray::fromBase64(data));
    return image;
}

QString Base64::textToBase64(const QString &text)
{
    return QString(text.toLocal8Bit().toBase64());
}

QString Base64::base64ToText(const QString &text)
{
    return QString(QByteArray::fromBase64(text.toLocal8Bit()));
}
