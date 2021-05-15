#include "quistyle.h"

void QUIStyle::getStyle(QStringList &styleNames, QStringList &styleFiles)
{
    static QStringList names;
    if (names.count() == 0) {
        names << "银色" << "蓝色" << "浅蓝色" << "深蓝色";
        names << "灰色" << "浅灰色" << "深灰色";
        names << "黑色" << "浅黑色" << "深黑色" << "PS黑色";
        names << "黑色扁平" << "白色扁平" << "蓝色扁平";
        names << "紫色" << "黑蓝色" << "视频黑";
    }

    //中文皮肤名称对应样式表文件
    static QStringList files;
    if (files.count() == 0) {
        files << ":/qss/silvery.css" << ":/qss/blue.css" << ":/qss/lightblue.css" << ":/qss/darkblue.css";
        files << ":/qss/gray.css" << ":/qss/lightgray.css" << ":/qss/darkgray.css";
        files << ":/qss/black.css" << ":/qss/lightblack.css" << ":/qss/darkblack.css" << ":/qss/psblack.css";
        files << ":/qss/flatblack.css" << ":/qss/flatwhite.css" << ":/qss/flatblue.css";
        files << ":/qss/purple.css" << ":/qss/blackblue.css" << ":/qss/blackvideo.css";
    }

    styleNames = names;
    styleFiles = files;
}

void QUIStyle::setStyle(const QString &qss, const QString &paletteColor)
{
    QUIHelper::isCustomUI = true;
    qApp->setPalette(QPalette(paletteColor));
    qApp->setStyleSheet(qss);
}

void QUIStyle::setStyle(const QUIStyle::Style &style)
{
    //取出所有的皮肤名称和对应样式文件
    QStringList styleNames, styleFiles;
    getStyle(styleNames, styleFiles);

    //取出对应索引的样式文件
    QString qssFile = (styleFiles.at((int)style));

    //设置全局样式
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                    QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);
        setStyle(qss, paletteColor);
        file.close();
    }
}

void QUIStyle::setStyle(const QString &qssFile)
{
    QString paletteColor, textColor;
    setStyle(qssFile, paletteColor, textColor);
}

void QUIStyle::setStyle(const QString &qssFile, QString &paletteColor, QString &textColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        paletteColor = qss.mid(20, 7);
        textColor = qss.mid(49, 7);
        getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                    QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);
        setStyle(qss, paletteColor);
        file.close();
    }
}

void QUIStyle::setStyle(const QString &qssFile, QString &textColor, QString &panelColor, QString &borderColor,
                        QString &normalColorStart, QString &normalColorEnd,
                        QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        getQssColor(qss, textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor);
        setStyle(qss, panelColor);
        file.close();
    }
}

void QUIStyle::getQssColor(const QString &qss, QString &textColor, QString &panelColor, QString &borderColor,
                           QString &normalColorStart, QString &normalColorEnd,
                           QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QUIHelper::isCustomUI = true;

    QString str = qss;
    QString flagTextColor = "TextColor:";
    int indexTextColor = str.indexOf(flagTextColor);
    if (indexTextColor >= 0) {
        textColor = str.mid(indexTextColor + flagTextColor.length(), 7);
    }

    QString flagPanelColor = "PanelColor:";
    int indexPanelColor = str.indexOf(flagPanelColor);
    if (indexPanelColor >= 0) {
        panelColor = str.mid(indexPanelColor + flagPanelColor.length(), 7);
    }

    QString flagBorderColor = "BorderColor:";
    int indexBorderColor = str.indexOf(flagBorderColor);
    if (indexBorderColor >= 0) {
        borderColor = str.mid(indexBorderColor + flagBorderColor.length(), 7);
    }

    QString flagNormalColorStart = "NormalColorStart:";
    int indexNormalColorStart = str.indexOf(flagNormalColorStart);
    if (indexNormalColorStart >= 0) {
        normalColorStart = str.mid(indexNormalColorStart + flagNormalColorStart.length(), 7);
    }

    QString flagNormalColorEnd = "NormalColorEnd:";
    int indexNormalColorEnd = str.indexOf(flagNormalColorEnd);
    if (indexNormalColorEnd >= 0) {
        normalColorEnd = str.mid(indexNormalColorEnd + flagNormalColorEnd.length(), 7);
    }

    QString flagDarkColorStart = "DarkColorStart:";
    int indexDarkColorStart = str.indexOf(flagDarkColorStart);
    if (indexDarkColorStart >= 0) {
        darkColorStart = str.mid(indexDarkColorStart + flagDarkColorStart.length(), 7);
    }

    QString flagDarkColorEnd = "DarkColorEnd:";
    int indexDarkColorEnd = str.indexOf(flagDarkColorEnd);
    if (indexDarkColorEnd >= 0) {
        darkColorEnd = str.mid(indexDarkColorEnd + flagDarkColorEnd.length(), 7);
    }

    QString flagHighColor = "HighColor:";
    int indexHighColor = str.indexOf(flagHighColor);
    if (indexHighColor >= 0) {
        highColor = str.mid(indexHighColor + flagHighColor.length(), 7);
    }
}

void QUIStyle::setLabStyle(QLabel *lab, quint8 type, const QString &bgColor, const QString &textColor)
{
    QString colorBg = bgColor;
    QString colorText = textColor;

    //如果设置了新颜色则启用新颜色
    if (bgColor.isEmpty() || textColor.isEmpty()) {
        if (type == 0) {
            colorBg = "#D64D54";
            colorText = "#FFFFFF";
        } else if (type == 1) {
            colorBg = "#17A086";
            colorText = "#FFFFFF";
        } else if (type == 2) {
            colorBg = "#47A4E9";
            colorText = "#FFFFFF";
        } else if (type == 3) {
            colorBg = "#282D30";
            colorText = "#FFFFFF";
        } else if (type == 4) {
            colorBg = "#0E99A0";
            colorText = "#FFFFFF";
        } else if (type == 5) {
            colorBg = "#A279C5";
            colorText = "#FFFFFF";
        } else if (type == 6) {
            colorBg = "#8C2957";
            colorText = "#FFFFFF";
        } else if (type == 7) {
            colorBg = "#04567E";
            colorText = "#FFFFFF";
        } else if (type == 8) {
            colorBg = "#FD8B28";
            colorText = "#FFFFFF";
        } else if (type == 9) {
            colorBg = "#5580A2";
            colorText = "#FFFFFF";
        }
    }

    QStringList qss;
    //禁用颜色
    qss << QString("QLabel::disabled{background:none;color:%1;}").arg(QUIConfig::BorderColor);
    //正常颜色
    qss << QString("QLabel{border:none;background-color:%1;color:%2;}").arg(colorBg).arg(colorText);
    lab->setStyleSheet(qss.join(""));
}

