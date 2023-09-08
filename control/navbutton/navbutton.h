#ifndef NAVBUTTON_H
#define NAVBUTTON_H

/**
 * 导航按钮控件 作者:feiyangqingyun(QQ:517216493) 2017-12-19
 * 1. 可设置文字的左侧、右侧、顶部、底部间隔。
 * 2. 可设置文字对齐方式。
 * 3. 可设置显示倒三角、倒三角边长、倒三角位置、倒三角颜色。
 * 4. 可设置显示图标、图标间隔、图标尺寸、正常状态图标、悬停状态图标、选中状态图标。
 * 5. 可设置显示边框线条、线条宽度、线条间隔、线条位置、线条颜色。
 * 6. 可设置正常背景颜色、悬停背景颜色、选中背景颜色。
 * 7. 可设置正常文字颜色、悬停文字颜色、选中文字颜色。
 * 8. 可设置背景颜色为画刷颜色。
 */

#include <QPushButton>

#ifdef quc
class Q_DECL_EXPORT NavButton : public QPushButton
#else
class NavButton : public QPushButton
#endif

{
    Q_OBJECT
    Q_ENUMS(TextAlign)
    Q_ENUMS(TrianglePosition)
    Q_ENUMS(LinePosition)
    Q_ENUMS(IconPosition)

    Q_PROPERTY(int paddingLeft READ getPaddingLeft WRITE setPaddingLeft)
    Q_PROPERTY(int paddingRight READ getPaddingRight WRITE setPaddingRight)
    Q_PROPERTY(int paddingTop READ getPaddingTop WRITE setPaddingTop)
    Q_PROPERTY(int paddingBottom READ getPaddingBottom WRITE setPaddingBottom)
    Q_PROPERTY(TextAlign textAlign READ getTextAlign WRITE setTextAlign)

    Q_PROPERTY(bool showTriangle READ getShowTriangle WRITE setShowTriangle)
    Q_PROPERTY(int triangleLen READ getTriangleLen WRITE setTriangleLen)
    Q_PROPERTY(TrianglePosition trianglePosition READ getTrianglePosition WRITE setTrianglePosition)
    Q_PROPERTY(QColor triangleColor READ getTriangleColor WRITE setTriangleColor)

    Q_PROPERTY(bool showIcon READ getShowIcon WRITE setShowIcon)
    Q_PROPERTY(int iconSpace READ getIconSpace WRITE setIconSpace)
    Q_PROPERTY(QSize iconSize READ getIconSize WRITE setIconSize)
    Q_PROPERTY(QPixmap iconNormal READ getIconNormal WRITE setIconNormal)
    Q_PROPERTY(QPixmap iconHover READ getIconHover WRITE setIconHover)
    Q_PROPERTY(QPixmap iconCheck READ getIconCheck WRITE setIconCheck)

    Q_PROPERTY(bool showLine READ getShowLine WRITE setShowLine)
    Q_PROPERTY(int lineSpace READ getLineSpace WRITE setLineSpace)
    Q_PROPERTY(int lineWidth READ getLineWidth WRITE setLineWidth)
    Q_PROPERTY(LinePosition linePosition READ getLinePosition WRITE setLinePosition)
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor)

    Q_PROPERTY(QColor normalBgColor READ getNormalBgColor WRITE setNormalBgColor)
    Q_PROPERTY(QColor hoverBgColor READ getHoverBgColor WRITE setHoverBgColor)
    Q_PROPERTY(QColor checkBgColor READ getCheckBgColor WRITE setCheckBgColor)
    Q_PROPERTY(QColor normalTextColor READ getNormalTextColor WRITE setNormalTextColor)
    Q_PROPERTY(QColor hoverTextColor READ getHoverTextColor WRITE setHoverTextColor)
    Q_PROPERTY(QColor checkTextColor READ getCheckTextColor WRITE setCheckTextColor)

public:
    enum TextAlign {
        TextAlign_Left = 0x0001,    //左侧对齐
        TextAlign_Right = 0x0002,   //右侧对齐
        TextAlign_Top = 0x0020,     //顶部对齐
        TextAlign_Bottom = 0x0040,  //底部对齐
        TextAlign_Center = 0x0004   //居中对齐
    };

    enum TrianglePosition {
        TrianglePosition_Left = 0,  //左侧
        TrianglePosition_Right = 1, //右侧
        TrianglePosition_Top = 2,   //顶部
        TrianglePosition_Bottom = 3 //底部
    };

    enum IconPosition {
        IconPosition_Left = 0,      //左侧
        IconPosition_Right = 1,     //右侧
        IconPosition_Top = 2,       //顶部
        IconPosition_Bottom = 3     //底部
    };

    enum LinePosition {
        LinePosition_Left = 0,      //左侧
        LinePosition_Right = 1,     //右侧
        LinePosition_Top = 2,       //顶部
        LinePosition_Bottom = 3     //底部
    };

    explicit NavButton(QWidget *parent = 0);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawText(QPainter *painter);
    void drawIcon(QPainter *painter);
    void drawLine(QPainter *painter);
    void drawTriangle(QPainter *painter);

private:
    int paddingLeft;            //文字左侧间隔
    int paddingRight;           //文字右侧间隔
    int paddingTop;             //文字顶部间隔
    int paddingBottom;          //文字底部间隔
    TextAlign textAlign;        //文字对齐

    bool showTriangle;          //显示倒三角
    int triangleLen;            //倒三角边长
    TrianglePosition trianglePosition;//倒三角位置
    QColor triangleColor;       //倒三角颜色

    bool showIcon;              //显示图标
    int iconSpace;              //图标间隔
    QSize iconSize;             //图标尺寸
    QPixmap iconNormal;         //正常图标
    QPixmap iconHover;          //悬停图标
    QPixmap iconCheck;          //选中图标

    bool showLine;              //显示线条
    int lineSpace;              //线条间隔
    int lineWidth;              //线条宽度
    LinePosition linePosition;  //线条位置
    QColor lineColor;           //线条颜色

    QColor normalBgColor;       //正常背景颜色
    QColor hoverBgColor;        //悬停背景颜色
    QColor checkBgColor;        //选中背景颜色
    QColor normalTextColor;     //正常文字颜色
    QColor hoverTextColor;      //悬停文字颜色
    QColor checkTextColor;      //选中文字颜色

    QBrush normalBgBrush;       //正常背景画刷
    QBrush hoverBgBrush;        //悬停背景画刷
    QBrush checkBgBrush;        //选中背景画刷

    bool hover;                 //悬停标志位

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置文字左侧间隔
    int getPaddingLeft() const;
    void setPaddingLeft(int paddingLeft);

    //获取和设置文字左侧间隔
    int getPaddingRight() const;
    void setPaddingRight(int paddingRight);

    //获取和设置文字顶部间隔
    int getPaddingTop() const;
    void setPaddingTop(int paddingTop);

    //获取和设置文字底部间隔
    int getPaddingBottom() const;
    void setPaddingBottom(int paddingBottom);

    //设置边距
    void setPadding(int padding);
    void setPadding(int paddingLeft, int paddingRight, int paddingTop, int paddingBottom);

    //获取和设置文字对齐
    TextAlign getTextAlign() const;
    void setTextAlign(const TextAlign &textAlign);

    //获取和设置显示倒三角
    bool getShowTriangle() const;
    void setShowTriangle(bool showTriangle);

    //获取和设置倒三角边长
    int getTriangleLen() const;
    void setTriangleLen(int triangleLen);

    //获取和设置倒三角位置
    TrianglePosition getTrianglePosition() const;
    void setTrianglePosition(const TrianglePosition &trianglePosition);

    //获取和设置倒三角颜色
    QColor getTriangleColor() const;
    void setTriangleColor(const QColor &triangleColor);

    //获取和设置显示图标
    bool getShowIcon() const;
    void setShowIcon(bool showIcon);

    //获取和设置图标间隔
    int getIconSpace() const;
    void setIconSpace(int iconSpace);

    //获取和设置图标尺寸
    QSize getIconSize() const;
    void setIconSize(const QSize &iconSize);

    //获取和设置正常图标
    QPixmap getIconNormal() const;
    void setIconNormal(const QPixmap &iconNormal);

    //获取和设置悬停图标
    QPixmap getIconHover() const;
    void setIconHover(const QPixmap &iconHover);

    //获取和设置按下图标
    QPixmap getIconCheck() const;
    void setIconCheck(const QPixmap &iconCheck);

    //获取和设置显示线条
    bool getShowLine() const;
    void setShowLine(bool showLine);

    //获取和设置线条间隔
    int getLineSpace() const;
    void setLineSpace(int lineSpace);

    //获取和设置线条宽度
    int getLineWidth() const;
    void setLineWidth(int lineWidth);

    //获取和设置线条位置
    LinePosition getLinePosition() const;
    void setLinePosition(const LinePosition &linePosition);

    //获取和设置线条颜色
    QColor getLineColor() const;
    void setLineColor(const QColor &lineColor);

    //获取和设置正常背景颜色
    QColor getNormalBgColor() const;
    void setNormalBgColor(const QColor &normalBgColor);

    //获取和设置悬停背景颜色
    QColor getHoverBgColor() const;
    void setHoverBgColor(const QColor &hoverBgColor);

    //获取和设置选中背景颜色
    QColor getCheckBgColor() const;
    void setCheckBgColor(const QColor &checkBgColor);

    //获取和设置正常文字颜色
    QColor getNormalTextColor() const;
    void setNormalTextColor(const QColor &normalTextColor);

    //获取和设置悬停文字颜色
    QColor getHoverTextColor() const;
    void setHoverTextColor(const QColor &hoverTextColor);

    //获取和设置选中文字颜色
    QColor getCheckTextColor() const;
    void setCheckTextColor(const QColor &checkTextColor);

    //设置正常背景画刷
    void setNormalBgBrush(const QBrush &normalBgBrush);
    //设置悬停背景画刷
    void setHoverBgBrush(const QBrush &hoverBgBrush);
    //设置选中背景画刷
    void setCheckBgBrush(const QBrush &checkBgBrush);
};

#endif // NAVBUTTON_H
