#ifndef BASE64HELPER_H
#define BASE64HELPER_H

/**
 * base64编码转换类 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1. 图片转base64字符串。
 * 2. base64字符串转图片。
 * 3. 字符转base64字符串。
 * 4. base64字符串转字符。
 * 5. 后期增加数据压缩。
 * 6. Qt6对base64编码转换进行了重写效率提升至少200%。
 */

#include <QImage>

#ifdef quc
class Q_DECL_EXPORT Base64Helper
#else
class Base64Helper
#endif

{
public:
    //图片转base64字符串
    static QString imageToBase64(const QImage &image);
    static QByteArray imageToBase64x(const QImage &image);

    //base64字符串转图片
    static QImage base64ToImage(const QString &data);
    static QImage base64ToImagex(const QByteArray &data);

    //字符串与base64互转
    static QString textToBase64(const QString &text);
    static QString base64ToText(const QString &text);
};

#endif // BASE64HELPER_H
