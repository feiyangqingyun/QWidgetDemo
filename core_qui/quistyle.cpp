#include "quistyle.h"

bool QUIStyle::isDark1(const QString &styleName)
{
    QStringList listDark;
    listDark << "blackvideo" << "blackblue" << "darkblack" << "darkblue" << "flatblack" << "flatblue" << "purple";
    bool dark = false;
    foreach (QString list, listDark) {
        if (styleName.contains(list)) {
            dark = true;
            break;
        }
    }
    return dark;
}

bool QUIStyle::isDark2(const QString &styleName)
{
    QStringList listDark;
    listDark << "blackvideo" << "blackblue" << "darkblack" << "darkblue" << "purple";
    bool dark = false;
    foreach (QString list, listDark) {
        if (styleName.contains(list)) {
            dark = true;
            break;
        }
    }
    return dark;
}

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

void QUIStyle::setStyle(const QString &qss)
{
    QStringList list;
    list << qss;

    //5.12开始tabbar左右反过来的
#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    //左右两侧的边框偏移一个像素
    list << "QTabWidget::pane:left{left:-1px;right:0px;}";
    list << "QTabWidget::pane:right{right:-1px;left:0px;}";
    //选中和悬停的时候边缘加深2个像素
    list << "QTabBar::tab:left:selected,QTabBar::tab:left:hover{border-width:0px 0px 0px 2px;}";
    list << "QTabBar::tab:right:selected,QTabBar::tab:right:hover{border-width:0px 2px 0px 0px;}";
#endif

    //5.2开始颜色支持透明度
    QString txtReadOnlyColor = QUIConfig::NormalColorStart.right(6);
#if (QT_VERSION >= QT_VERSION_CHECK(5,2,0))
    txtReadOnlyColor = "88" + txtReadOnlyColor;
#endif

    //增加文本框只读背景颜色
    list << QString("QLineEdit:read-only{background-color:#%1;}").arg(txtReadOnlyColor);

    QUIHelper::isCustomUI = true;
    //阴影边框和配色方案自动变化
    QUIHelper::setFormShadow(QUIConfig::HighColor);

    QString paletteColor = qss.mid(20, 7);
    qApp->setPalette(QPalette(paletteColor));
    qApp->setStyleSheet(list.join(""));
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
        setStyle(qss);
        file.close();
    }
}

void QUIStyle::setStyleFile(const QString &qssFile)
{
    QString paletteColor, textColor;
    setStyleFile(qssFile, paletteColor, textColor);
}

void QUIStyle::setStyleFile(const QString &qssFile, QString &paletteColor, QString &textColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        paletteColor = qss.mid(20, 7);
        textColor = qss.mid(49, 7);
        getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                    QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);
        setStyle(qss);
        file.close();
    }
}

void QUIStyle::setStyleFile(const QString &qssFile, QString &textColor, QString &panelColor, QString &borderColor,
                            QString &normalColorStart, QString &normalColorEnd,
                            QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        getQssColor(qss, textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor);
        setStyle(qss);
        file.close();
    }
}

void QUIStyle::getQssColor(const QString &qss, const QString &flag, QString &color)
{
    int index = qss.indexOf(flag);
    if (index >= 0) {
        color = qss.mid(index + flag.length(), 7);
    }
    //qDebug() << TIMEMS << flag << color;
}

void QUIStyle::getQssColor(const QString &qss, QString &textColor,
                           QString &panelColor, QString &borderColor,
                           QString &normalColorStart, QString &normalColorEnd,
                           QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    getQssColor(qss, "TextColor:", textColor);
    getQssColor(qss, "PanelColor:", panelColor);
    getQssColor(qss, "BorderColor:", borderColor);
    getQssColor(qss, "NormalColorStart:", normalColorStart);
    getQssColor(qss, "NormalColorEnd:", normalColorEnd);
    getQssColor(qss, "DarkColorStart:", darkColorStart);
    getQssColor(qss, "DarkColorEnd:", darkColorEnd);
    getQssColor(qss, "HighColor:", highColor);

    QUIHelper::isCustomUI = true;
    QUIConfig::TextColor = textColor;
    QUIConfig::PanelColor = panelColor;
    QUIConfig::BorderColor = borderColor;
    QUIConfig::NormalColorStart = normalColorStart;
    QUIConfig::NormalColorEnd = normalColorEnd;
    QUIConfig::DarkColorStart = darkColorStart;
    QUIConfig::DarkColorEnd = darkColorEnd;
    QUIConfig::HighColor = highColor;
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

