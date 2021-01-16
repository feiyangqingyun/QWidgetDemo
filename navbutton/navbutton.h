#ifndef NAVBUTTON_H
#define NAVBUTTON_H

/**
 * 导航按钮控件 作者:feiyangqingyun(QQ:517216493) 2017-12-19
 * 1. 可设置文字的左侧+右侧+顶部+底部间隔
 * 2. 可设置文字对齐方式
 * 3. 可设置显示倒三角/倒三角边长/倒三角位置/倒三角颜色
 * 4. 可设置显示图标/图标间隔/图标尺寸/正常状态图标/悬停状态图标/选中状态图标
 * 5. 可设置显示边框线条/线条宽度/线条间隔/线条位置/线条颜色
 * 6. 可设置正常背景颜色/悬停背景颜色/选中背景颜色
 * 7. 可设置正常文字颜色/悬停文字颜色/选中文字颜色
 * 8. 可设置背景颜色为画刷颜色
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
    int paddingLeft;                    //文字左侧间隔
    int paddingRight;                   //文字右侧间隔
    int paddingTop;                     //文字顶部间隔
    int paddingBottom;                  //文字底部间隔
    TextAlign textAlign;                //文字对齐

    bool showTriangle;                  //显示倒三角
    int triangleLen;                    //倒三角边长
    TrianglePosition trianglePosition;  //倒三角位置
    QColor triangleColor;               //倒三角颜色

    bool showIcon;                      //显示图标
    int iconSpace;                      //图标间隔
    QSize iconSize;                     //图标尺寸
    QPixmap iconNormal;                 //正常图标
    QPixmap iconHover;                  //悬停图标
    QPixmap iconCheck;                  //选中图标

    bool showLine;                      //显示线条
    int lineSpace;                      //线条间隔
    int lineWidth;                      //线条宽度
    LinePosition linePosition;          //线条位置
    QColor lineColor;                   //线条颜色

    QColor normalBgColor;               //正常背景颜色
    QColor hoverBgColor;                //悬停背景颜色
    QColor checkBgColor;                //选中背景颜色
    QColor normalTextColor;             //正常文字颜色
    QColor hoverTextColor;              //悬停文字颜色
    QColor checkTextColor;              //选中文字颜色

    QBrush normalBgBrush;               //正常背景画刷
    QBrush hoverBgBrush;                //悬停背景画刷
    QBrush checkBgBrush;                //选中背景画刷

    bool hover;                         //悬停标志位

public:
    int getPaddingLeft()                const;
    int getPaddingRight()               const;
    int getPaddingTop()                 const;
    int getPaddingBottom()              const;
    TextAlign getTextAlign()            const;

    bool getShowTriangle()              const;
    int getTriangleLen()                const;
    TrianglePosition getTrianglePosition()const;
    QColor getTriangleColor()           const;

    bool getShowIcon()                  const;
    int getIconSpace()                  const;
    QSize getIconSize()                 const;
    QPixmap getIconNormal()             const;
    QPixmap getIconHover()              const;
    QPixmap getIconCheck()              const;

    bool getShowLine()                  const;
    int getLineSpace()                  const;
    int getLineWidth()                  const;
    LinePosition getLinePosition()      const;
    QColor getLineColor()               const;

    QColor getNormalBgColor()           const;
    QColor getHoverBgColor()            const;
    QColor getCheckBgColor()            const;
    QColor getNormalTextColor()         const;
    QColor getHoverTextColor()          const;
    QColor getCheckTextColor()          const;

    QSize sizeHint()                    const;
    QSize minimumSizeHint()             const;

public Q_SLOTS:
    //设置文字间隔
    void setPaddingLeft(int paddingLeft);
    void setPaddingRight(int paddingRight);
    void setPaddingTop(int paddingTop);
    void setPaddingBottom(int paddingBottom);
    void setPadding(int padding);
    void setPadding(int paddingLeft, int paddingRight, int paddingTop, int paddingBottom);

    //设置文字对齐
    void setTextAlign(const TextAlign &textAlign);

    //设置显示倒三角
    void setShowTriangle(bool showTriangle);
    //设置倒三角边长
    void setTriangleLen(int triangleLen);
    //设置倒三角位置
    void setTrianglePosition(const TrianglePosition &trianglePosition);
    //设置倒三角颜色
    void setTriangleColor(const QColor &triangleColor);

    //设置显示图标
    void setShowIcon(bool showIcon);
    //设置图标间隔
    void setIconSpace(int iconSpace);
    //设置图标尺寸
    void setIconSize(const QSize &iconSize);
    //设置正常图标
    void setIconNormal(const QPixmap &iconNormal);
    //设置悬停图标
    void setIconHover(const QPixmap &iconHover);
    //设置按下图标
    void setIconCheck(const QPixmap &iconCheck);

    //设置显示线条
    void setShowLine(bool showLine);
    //设置线条间隔
    void setLineSpace(int lineSpace);
    //设置线条宽度
    void setLineWidth(int lineWidth);
    //设置线条位置
    void setLinePosition(const LinePosition &linePosition);
    //设置线条颜色
    void setLineColor(const QColor &lineColor);

    //设置正常背景颜色
    void setNormalBgColor(const QColor &normalBgColor);
    //设置悬停背景颜色
    void setHoverBgColor(const QColor &hoverBgColor);
    //设置选中背景颜色
    void setCheckBgColor(const QColor &checkBgColor);

    //设置正常文字颜色
    void setNormalTextColor(const QColor &normalTextColor);
    //设置悬停文字颜色
    void setHoverTextColor(const QColor &hoverTextColor);
    //设置选中文字颜色
    void setCheckTextColor(const QColor &checkTextColor);

    //设置正常背景画刷
    void setNormalBgBrush(const QBrush &normalBgBrush);
    //设置悬停背景画刷
    void setHoverBgBrush(const QBrush &hoverBgBrush);
    //设置选中背景画刷
    void setCheckBgBrush(const QBrush &checkBgBrush);

};

#endif // NAVBUTTON_H
