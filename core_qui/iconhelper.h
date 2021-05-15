#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QtCore>
#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#ifdef quc
class Q_DECL_EXPORT IconHelper : public QObject
#else
class IconHelper : public QObject
#endif

{
    Q_OBJECT

public:
    static IconHelper *Instance();
    explicit IconHelper(QObject *parent = 0);

    //获取图形字体
    QFont getIconFont();

    //设置图形字体到标签
    void setIcon(QLabel *lab, const QChar &icon, quint32 size = 12);
    //设置图形字体到按钮
    void setIcon(QAbstractButton *btn, const QChar &icon, quint32 size = 12);

    //获取指定图形字体,可以指定文字大小,图片宽高,文字对齐
    QPixmap getPixmap(const QColor &color, const QChar &icon, quint32 size = 12,
                      quint32 pixWidth = 15, quint32 pixHeight = 15,
                      int flags = Qt::AlignCenter);

    //根据按钮获取该按钮对应的图标
    QPixmap getPixmap(QToolButton *btn, bool normal);
    QPixmap getPixmap(QToolButton *btn, int type);

    //指定QFrame导航按钮样式,带图标
    void setStyle(QFrame *frame, QList<QToolButton *> btns, QList<QChar> icons,
                  quint32 iconSize = 12, quint32 iconWidth = 15, quint32 iconHeight = 15,
                  const QString &normalBgColor = "#2FC5A2",
                  const QString &darkBgColor = "#3EA7E9",
                  const QString &normalTextColor = "#EEEEEE",
                  const QString &darkTextColor = "#FFFFFF");

    //指定导航面板样式,不带图标
    static void setStyle(QWidget *widget, const QString &type = "left", int borderWidth = 3,
                         const QString &borderColor = "#029FEA",
                         const QString &normalBgColor = "#292F38",
                         const QString &darkBgColor = "#1D2025",
                         const QString &normalTextColor = "#54626F",
                         const QString &darkTextColor = "#FDFDFD");

    //移除导航面板样式,防止重复
    void removeStyle(QList<QToolButton *> btns);

    //指定QWidget导航面板样式,带图标和效果切换
    void setStyle(QWidget *widget, QList<QToolButton *> btns, QList<QChar> icons,
                  quint32 iconSize = 12, quint32 iconWidth = 15, quint32 iconHeight = 15,
                  const QString &type = "left", int borderWidth = 3,
                  const QString &borderColor = "#029FEA",
                  const QString &normalBgColor = "#292F38",
                  const QString &darkBgColor = "#1D2025",
                  const QString &normalTextColor = "#54626F",
                  const QString &darkTextColor = "#FDFDFD");

    struct StyleColor {
        quint32 iconSize;
        quint32 iconWidth;
        quint32 iconHeight;
        quint32 borderWidth;
        QString type;
        QString borderColor;
        QString normalBgColor;
        QString normalTextColor;
        QString hoverBgColor;
        QString hoverTextColor;
        QString pressedBgColor;
        QString pressedTextColor;
        QString checkedBgColor;
        QString checkedTextColor;

        StyleColor() {
            iconSize = 12;
            iconWidth = 15;
            iconHeight = 15;
            borderWidth = 3;
            type = "left";
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
    };

    //指定QWidget导航面板样式,带图标和效果切换+悬停颜色+按下颜色+选中颜色
    void setStyle(QWidget *widget, QList<QToolButton *> btns, QList<QChar> icons, const StyleColor &styleColor);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<IconHelper> self;

    QFont iconFont;             //图形字体
    QList<QToolButton *> btns;  //按钮队列
    QList<QPixmap> pixNormal;   //正常图片队列
    QList<QPixmap> pixDark;     //加深图片队列
    QList<QPixmap> pixHover;    //悬停图片队列
    QList<QPixmap> pixPressed;  //按下图片队列
    QList<QPixmap> pixChecked;  //选中图片队列
};

#endif // ICONHELPER_H
