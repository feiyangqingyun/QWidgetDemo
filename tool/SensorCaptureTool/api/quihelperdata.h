#ifndef QUIHELPERDATA_H
#define QUIHELPERDATA_H

#include <QObject>

class QUIHelperData
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

    //int转字节数组
    static QByteArray intToByte(int data);
    static QByteArray intToByteRec(int data);

    //字节数组转int
    static int byteToInt(const QByteArray &data);
    static int byteToIntRec(const QByteArray &data);
    static quint32 byteToUInt(const QByteArray &data);
    static quint32 byteToUIntRec(const QByteArray &data);

    //ushort转字节数组
    static QByteArray ushortToByte(ushort data);
    static QByteArray ushortToByteRec(ushort data);

    //字节数组转ushort
    static int byteToUShort(const QByteArray &data);
    static int byteToUShortRec(const QByteArray &data);

    //字符串补全
    static QString getValue(quint8 value);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString getXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static uchar getOrCode(const QByteArray &data);
    //计算校验码
    static uchar getCheckCode(const QByteArray &data);

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

#endif // QUIHELPERDATA_H
