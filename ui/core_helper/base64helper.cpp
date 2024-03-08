#include "base64helper.h"
#include "qbuffer.h"
#include "qdebug.h"

QString Base64Helper::imageToBase64(const QImage &image)
{
    return QString(imageToBase64x(image));
}

QByteArray Base64Helper::imageToBase64x(const QImage &image)
{
    //这个转换可能比较耗时建议在线程中执行
    QByteArray data;
    QBuffer buffer(&data);
    image.save(&buffer, "JPG");
    data = data.toBase64();
    return data;
}

QImage Base64Helper::base64ToImage(const QString &data)
{
    return base64ToImagex(data.toUtf8());
}

QImage Base64Helper::base64ToImagex(const QByteArray &data)
{
    //这个转换可能比较耗时建议在线程中执行
    QImage image;
    image.loadFromData(QByteArray::fromBase64(data));
    return image;
}

QString Base64Helper::textToBase64(const QString &text)
{
    return QString(text.toUtf8().toBase64());
}

QString Base64Helper::base64ToText(const QString &text)
{
    return QString(QByteArray::fromBase64(text.toUtf8()));
}
