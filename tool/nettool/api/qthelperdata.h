#ifndef QTHELPERDATA_H
#define QTHELPERDATA_H

#include <QObject>

class QtHelperData
{
public:
    //16进制字符串转10进制
    static int strHexToDecimal(const QString &strHex);
    //10进制字符串转10进制
    static int strDecimalToDecimal(const QString &strDecimal);
    //2进制字符串转10进制
    static int strBinToDecimal(const QString &strBin);

    //16进制字符串转2进制字符串
    static QString strHexToStrBin(const QString &strHex);
    //10进制转2进制字符串一个字节
    static QString decimalToStrBin1(int decimal);
    //10进制转2进制字符串两个字节
    static QString decimalToStrBin2(int decimal);
    //10进制转16进制字符串,补零.
    static QString decimalToStrHex(int decimal);

    //int和字节数组互转
    static QByteArray intToByte(int data, bool reverse = false);
    static int byteToInt(const QByteArray &data, bool reverse = false);

    //ushort和字节数组互转
    static QByteArray ushortToByte(int data, bool reverse = false);
    static int byteToShort(const QByteArray &data, bool reverse = false);

    //字符串补全
    static QString getValue(quint8 value);
    //字符串去空格 -1=移除左侧空格 0=移除所有空格 1=移除右侧空格 2=移除首尾空格 3=首尾清除中间留一个空格
    static QString trimmed(const QString &text, int type);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString getXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static quint8 getOrCode(const QByteArray &data);

    //公司专用-计算校验码
    static quint8 getCheckCode(const QByteArray &data);
    //公司专用-加上桢头和校验码完整数据
    static void getFullData(QByteArray &buffer);

    //CRC校验
    static quint16 getCrc16Rec(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getCrc16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getModbus16(quint8 *data, int len);
    static QByteArray getCrcCode(const QByteArray &data);

    //字节数组与Ascii字符串互转
    static void initAscii();
    static QString byteArrayToAsciiStr(const QByteArray &data);
    static QByteArray asciiStrToByteArray(const QString &data);

    //16进制字符串与字节数组互转
    static char hexStrToChar(char data);
    static QByteArray hexStrToByteArray(const QString &data);
    static QString byteArrayToHexStr(const QByteArray &data);
};

#endif // QTHELPERDATA_H
