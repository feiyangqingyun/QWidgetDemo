#ifndef QUISTYLE_H
#define QUISTYLE_H

#include "quihead.h"

class QUIStyle
{
public:
    //样式枚举
    enum Style {
        Style_Silvery = 0,      //银色样式
        Style_Blue = 1,         //蓝色样式
        Style_LightBlue = 2,    //淡蓝色样式
        Style_DarkBlue = 3,     //深蓝色样式
        Style_Gray = 4,         //灰色样式
        Style_LightGray = 5,    //浅灰色样式
        Style_DarkGray = 6,     //深灰色样式
        Style_Black = 7,        //黑色样式
        Style_LightBlack = 8,   //浅黑色样式
        Style_DarkBlack = 9,    //深黑色样式
        Style_PSBlack = 10,     //PS黑色样式
        Style_FlatBlack = 11,   //黑色扁平样式
        Style_FlatWhite = 12,   //白色扁平样式
        Style_FlatBlue = 13,    //蓝色扁平样式
        Style_Purple = 14,      //紫色样式
        Style_BlackBlue = 15,   //黑蓝色样式
        Style_BlackVideo = 16   //视频监控黑色样式
    };

    //获取皮肤样式中文名称和对应的样式表文件
    static void getStyle(QStringList &styleNames, QStringList &styleFiles);
    //设置全局样式
    static void setStyle(const QString &qss, const QString &paletteColor);
    static void setStyle(const QUIStyle::Style &style);
    static void setStyle(const QString &qssFile);
    static void setStyle(const QString &qssFile, QString &paletteColor, QString &textColor);
    static void setStyle(const QString &qssFile, QString &textColor,
                         QString &panelColor, QString &borderColor,
                         QString &normalColorStart, QString &normalColorEnd,
                         QString &darkColorStart, QString &darkColorEnd,
                         QString &highColor);

    //根据QSS样式获取对应颜色值
    static void getQssColor(const QString &qss, QString &textColor,
                            QString &panelColor, QString &borderColor,
                            QString &normalColorStart, QString &normalColorEnd,
                            QString &darkColorStart, QString &darkColorEnd,
                            QString &highColor);

    //设置标签颜色
    static void setLabStyle(QLabel *lab, quint8 type, const QString &bgColor = "", const QString &textColor = "");
};

#endif // QUISTYLE_H
