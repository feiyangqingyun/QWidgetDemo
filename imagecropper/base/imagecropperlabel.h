#ifndef IMAGECROPPERLABEL_H
#define IMAGECROPPERLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QPen>

enum class CropperShape {
    UNDEFINED = 0,
    RECT = 1,
    SQUARE = 2,
    FIXED_RECT = 3,
    ELLIPSE = 4,
    CIRCLE = 5,
    FIXED_ELLIPSE = 6
};

enum class OutputShape {
    RECT = 0,
    ELLIPSE = 1
};

enum class SizeType {
    fixedSize = 0,
    fitToMaxWidth = 1,
    fitToMaxHeight = 2,
    fitToMaxWidthHeight = 3,
};

class ImageCropperLabel : public QLabel
{
    Q_OBJECT
public:
    ImageCropperLabel(int width, int height, QWidget *parent);

    void setOriginalImage(const QPixmap &pixmap);
    void setOutputShape(OutputShape shape) { outputShape = shape; }
    QPixmap getCroppedImage();
    QPixmap getCroppedImage(OutputShape shape);

    /*****************************************
     * Set cropper's shape
    *****************************************/
    void setRectCropper();
    void setSquareCropper();
    void setEllipseCropper();
    void setCircleCropper();
    void setFixedRectCropper(QSize size);
    void setFixedEllipseCropper(QSize size);
    void setCropper(CropperShape shape, QSize size); // not recommended

    /*****************************************************************************
     * Set cropper's fixed size
    *****************************************************************************/
    void setCropperFixedSize(int fixedWidth, int fixedHeight);
    void setCropperFixedWidth(int fixedWidht);
    void setCropperFixedHeight(int fixedHeight);

    /*****************************************************************************
     * Set cropper's minimum size
     * default: the twice of minimum of the edge lenght of drag square
    *****************************************************************************/
    void setCropperMinimumSize(int minWidth, int minHeight)
    {
        cropperMinimumWidth = minWidth;
        cropperMinimumHeight = minHeight;
    }
    void setCropperMinimumWidth(int minWidth) { cropperMinimumWidth = minWidth; }
    void setCropperMinimumHeight(int minHeight) { cropperMinimumHeight = minHeight; }

    /*************************************************
     * Set the size, color, visibility of rectangular border
    *************************************************/
    void setShowRectBorder(bool show) { isShowRectBorder = show; }
    QPen getBorderPen() { return borderPen; }
    void setBorderPen(const QPen &pen) { borderPen = pen; }

    /*************************************************
     * Set the size, color of drag square
    *************************************************/
    void setShowDragSquare(bool show) { isShowDragSquare = show; }
    void setDragSquareEdge(int edge) { dragSquareEdge = (edge >= 3 ? edge : 3); }
    void setDragSquareColor(const QColor &color) { dragSquareColor = color; }

    /*****************************************
     *  Opacity Effect
    *****************************************/
    void enableOpacity(bool b = true) { isShowOpacityEffect = b; }
    void setOpacity(double newOpacity) { opacity = newOpacity; }

signals:
    void croppedImageChanged();

protected:
    /*****************************************
     * Event
    *****************************************/
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    /***************************************
     * Draw shapes
    ***************************************/
    void drawFillRect(QPoint centralPoint, int edge, QColor color);
    void drawRectOpacity();
    void drawEllipseOpacity();
    void drawOpacity(const QPainterPath &path); // shadow effect
    void drawSquareEdge(bool onlyFourCorners);

    /***************************************
     * Other utility methods
    ***************************************/
    int getPosInCropperRect(const QPoint &pt);
    bool isPosNearDragSquare(const QPoint &pt1, const QPoint &pt2);
    void resetCropperPos();
    void changeCursor();

    enum {
        RECT_OUTSIZD = 0,
        RECT_INSIDE = 1,
        RECT_TOP_LEFT,
        RECT_TOP,
        RECT_TOP_RIGHT,
        RECT_RIGHT,
        RECT_BOTTOM_RIGHT,
        RECT_BOTTOM,
        RECT_BOTTOM_LEFT,
        RECT_LEFT
    };

    const bool ONLY_FOUR_CORNERS = true;

private:
    QPixmap originalImage;
    QPixmap tempImage;

    bool isShowRectBorder = true;
    QPen borderPen;

    CropperShape cropperShape = CropperShape::UNDEFINED;
    OutputShape outputShape = OutputShape::RECT;

    QRect imageRect; // the whole image area in the label (not real size)
    QRect cropperRect; // a rectangle frame to choose image area (not real size)
    QRect cropperRect_; // cropper rect (real size)
    double scaledRate = 1.0;

    bool isLButtonPressed = false;
    bool isCursorPosCalculated = false;
    int cursorPosInCropperRect = RECT_OUTSIZD;
    QPoint lastPos;
    QPoint currPos;

    bool isShowDragSquare = true;
    int dragSquareEdge = 8;
    QColor dragSquareColor = Qt::white;

    int cropperMinimumWidth = dragSquareEdge * 2;
    int cropperMinimumHeight = dragSquareEdge * 2;

    bool isShowOpacityEffect = false;
    double opacity = 0.6;
};

#endif // IMAGECROPPERLABEL_H
