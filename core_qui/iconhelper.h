#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

/**
 * 超级图形字体类 作者:feiyangqingyun(QQ:517216493) 2016-11-23
 * 1. 可传入多种图形字体文件
 * 2. 可设置 QLabel+QAbstractButton 图形字体
 * 3. 可设置按钮图标
 * 4. 可获取指定尺寸的图形字体图片
 * 5. 超级导航栏样式设置,带图标和效果切换+悬停颜色+按下颜色+选中颜色
 */

#ifdef quc
class Q_DECL_EXPORT IconHelper : public QObject
#else
class IconHelper : public QObject
#endif

{
    Q_OBJECT

public:
    //样式颜色结构体
    struct StyleColor {
        QString position;           //位置 left right top bottom
        bool textBesideIcon;        //文字在图标下面

        quint32 iconSize;           //图标字体尺寸
        quint32 iconWidth;          //图标图片宽度
        quint32 iconHeight;         //图标图片高度

        quint32 borderWidth;        //边框宽度
        QString borderColor;        //边框颜色

        QString normalBgColor;      //正常背景颜色
        QString normalTextColor;    //正常文字颜色
        QString hoverBgColor;       //悬停背景颜色
        QString hoverTextColor;     //悬停文字颜色
        QString pressedBgColor;     //按下背景颜色
        QString pressedTextColor;   //按下文字颜色
        QString checkedBgColor;     //选中背景颜色
        QString checkedTextColor;   //选中文字颜色

        StyleColor() {
            position = "left";
            textBesideIcon = false;

            iconSize = 12;
            iconWidth = 15;
            iconHeight = 15;

            borderWidth = 3;
            borderColor = "#029FEA";

            normalBgColor = "#292F38";
            normalTextColor = "#54626F";
            hoverBgColor = "#40444D";
            hoverTextColor = "#FDFDFD";
            pressedBgColor = "#404244";
            pressedTextColor = "#FDFDFD";
            checkedBgColor = "#44494F";
            checkedTextColor = "#FDFDFD";
        }

        //设置常规颜色 普通状态+加深状态
        void setColor(const QString &normalBgColor,
                      const QString &normalTextColor,
                      const QString &darkBgColor,
                      const QString &darkTextColor) {
            this->normalBgColor = normalBgColor;
            this->normalTextColor = normalTextColor;
            this->hoverBgColor = darkBgColor;
            this->hoverTextColor = darkTextColor;
            this->pressedBgColor = darkBgColor;
            this->pressedTextColor = darkTextColor;
            this->checkedBgColor = darkBgColor;
            this->checkedTextColor = darkTextColor;
        }
    };

    //阿里巴巴图形字体类
    static IconHelper *iconFontAliBaBa;
    //FontAwesome图形字体类
    static IconHelper *iconFontAwesome;
    //初始化图形字体
    static void initFont();

    static void setIcon(QLabel *lab, int icon, quint32 size = 12);
    static void setIcon(QAbstractButton *btn, int icon, quint32 size = 12);

    static void setPixmap(QAbstractButton *btn, const QColor &color,
                          int icon, quint32 size = 12,
                          quint32 width = 15, quint32 height = 15,
                          int flags = Qt::AlignCenter);
    static QPixmap getPixmap(const QColor &color, int icon, quint32 size = 12,
                             quint32 width = 15, quint32 height = 15,
                             int flags = Qt::AlignCenter);

    static void setStyle(QWidget *widget, QList<QPushButton *> btns, QList<int> icons, const StyleColor &styleColor);
    static void setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> icons, const StyleColor &styleColor);
    static void setStyle(QWidget *widget, QList<QAbstractButton *> btns, QList<int> icons, const StyleColor &styleColor);

    //默认构造函数,传入字体文件+字体名称
    explicit IconHelper(const QString &fontFile, const QString &fontName, QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QFont iconFont;                 //图形字体
    QList<QAbstractButton *> btns;  //按钮队列
    QList<QPixmap> pixNormal;       //正常图片队列
    QList<QPixmap> pixHover;        //悬停图片队列
    QList<QPixmap> pixPressed;      //按下图片队列
    QList<QPixmap> pixChecked;      //选中图片队列

private slots:
    //按钮选中状态切换处理
    void toggled(bool checked);

public:
    //设置图形字体到标签
    void setIcon1(QLabel *lab, int icon, quint32 size = 12);
    //设置图形字体到按钮
    void setIcon1(QAbstractButton *btn, int icon, quint32 size = 12);

    //设置图形字体到图标
    void setPixmap1(QAbstractButton *btn, const QColor &color,
                    int icon, quint32 size = 12,
                    quint32 width = 15, quint32 height = 15,
                    int flags = Qt::AlignCenter);
    //获取指定图形字体,可以指定文字大小,图片宽高,文字对齐
    QPixmap getPixmap1(const QColor &color, int icon, quint32 size = 12,
                       quint32 width = 15, quint32 height = 15,
                       int flags = Qt::AlignCenter);

    //指定导航面板样式,带图标和效果切换+悬停颜色+按下颜色+选中颜色
    void setStyle1(QWidget *widget, QList<QPushButton *> btns, QList<int> icons, const StyleColor &styleColor);
    void setStyle1(QWidget *widget, QList<QToolButton *> btns, QList<int> icons, const StyleColor &styleColor);
    void setStyle1(QWidget *widget, QList<QAbstractButton *> btns, QList<int> icons, const StyleColor &styleColor);
};

#endif // ICONHELPER_H
