#include "imagecropperlabel.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDebug>
#include <QBitmap>

ImageCropperLabel::ImageCropperLabel(int width, int height, QWidget* parent) :
         QLabel(parent)
{
    this->setFixedSize(width, height);
    this->setAlignment(Qt::AlignCenter);
    this->setMouseTracking(true);

    borderPen.setWidth(1);
    borderPen.setColor(Qt::white);
    borderPen.setDashPattern(QVector<qreal>() << 3 << 3 << 3 << 3);
}

void ImageCropperLabel::setOriginalImage(const QPixmap &pixmap) {
    originalImage = pixmap;

    int imgWidth = pixmap.width();
    int imgHeight = pixmap.height();
    int labelWidth = this->width();
    int labelHeight = this->height();
    int imgWidthInLabel;
    int imgHeightInLabel;

    if (imgWidth * labelHeight < imgHeight * labelWidth) {
        scaledRate = labelHeight / double(imgHeight);
        imgHeightInLabel = labelHeight;
        imgWidthInLabel = int(scaledRate * imgWidth);
        imageRect.setRect((labelWidth - imgWidthInLabel) / 2, 0,
                          imgWidthInLabel, imgHeightInLabel);
    }
    else {
        scaledRate = labelWidth / double(imgWidth);
        imgWidthInLabel = labelWidth;
        imgHeightInLabel = int(scaledRate * imgHeight);
        imageRect.setRect(0, (labelHeight - imgHeightInLabel) / 2,
                          imgWidthInLabel, imgHeightInLabel);
    }

    tempImage = originalImage.scaled(imgWidthInLabel, imgHeightInLabel,
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setPixmap(tempImage);

    if (cropperShape >= CropperShape::FIXED_RECT) {
        cropperRect.setWidth(int(cropperRect_.width() * scaledRate));
        cropperRect.setHeight(int(cropperRect_.height() * scaledRate));
    }
    resetCropperPos();
}


/*****************************************
 * set cropper's shape (and size)
*****************************************/
void ImageCropperLabel::setRectCropper() {
    cropperShape = CropperShape::RECT;
    resetCropperPos();
}

void ImageCropperLabel::setSquareCropper() {
    cropperShape = CropperShape::SQUARE;
    resetCropperPos();
}

void ImageCropperLabel::setEllipseCropper() {
    cropperShape = CropperShape::ELLIPSE;
    resetCropperPos();
}

void ImageCropperLabel::setCircleCropper() {
    cropperShape = CropperShape::CIRCLE;
    resetCropperPos();
}

void ImageCropperLabel::setFixedRectCropper(QSize size) {
    cropperShape = CropperShape::FIXED_RECT;
    cropperRect_.setSize(size);
    resetCropperPos();
}

void ImageCropperLabel::setFixedEllipseCropper(QSize size) {
    cropperShape = CropperShape::FIXED_ELLIPSE;
    cropperRect_.setSize(size);
    resetCropperPos();
}

// not recommended
void ImageCropperLabel::setCropper(CropperShape shape, QSize size) {
    cropperShape = shape;
    cropperRect_.setSize(size);
    resetCropperPos();
}

/*****************************************************************************
     * Set cropper's fixed size
    *****************************************************************************/
void ImageCropperLabel::setCropperFixedSize(int fixedWidth, int fixedHeight) {
    cropperRect_.setSize(QSize(fixedWidth, fixedHeight));
    resetCropperPos();
}

void ImageCropperLabel::setCropperFixedWidth(int fixedWidth) {
    cropperRect_.setWidth(fixedWidth);
    resetCropperPos();
}

void ImageCropperLabel::setCropperFixedHeight(int fixedHeight) {
    cropperRect_.setHeight(fixedHeight);
    resetCropperPos();
}

/**********************************************
 * Move cropper to the center of the image
 * And resize to default
**********************************************/
void ImageCropperLabel::resetCropperPos() {
    int labelWidth = this->width();
    int labelHeight = this->height();

    if (cropperShape == CropperShape::FIXED_RECT || cropperShape == CropperShape::FIXED_ELLIPSE) {
        cropperRect.setWidth(int(cropperRect_.width() * scaledRate));
        cropperRect.setHeight(int(cropperRect_.height() * scaledRate));
    }

    switch (cropperShape) {
        case CropperShape::UNDEFINED:
            break;
        case CropperShape::FIXED_RECT:
        case CropperShape::FIXED_ELLIPSE: {
            cropperRect.setRect((labelWidth - cropperRect.width()) / 2,
                             (labelHeight - cropperRect.height()) / 2,
                             cropperRect.width(), cropperRect.height());
            break;
        }
        case CropperShape::RECT:
        case CropperShape::SQUARE:
        case CropperShape::ELLIPSE:
        case CropperShape::CIRCLE: {
            int imgWidth = tempImage.width();
            int imgHeight = tempImage.height();
            int edge = int((imgWidth > imgHeight ? imgHeight : imgWidth) * 3 / 4.0);
            cropperRect.setRect((labelWidth - edge) / 2, (labelHeight - edge) / 2, edge, edge);
            break;
        }
    }
}

QPixmap ImageCropperLabel::getCroppedImage() {
    return getCroppedImage(this->outputShape);
}

QPixmap ImageCropperLabel::getCroppedImage(OutputShape shape) {
    int startX = int((cropperRect.left() - imageRect.left()) / scaledRate);
    int startY = int((cropperRect.top() - imageRect.top()) / scaledRate);
    int croppedWidth = int(cropperRect.width() / scaledRate);
    int croppedHeight = int(cropperRect.height() / scaledRate);

    QPixmap resultImage(croppedWidth, croppedHeight);
    resultImage = originalImage.copy(startX, startY, croppedWidth, croppedHeight);

    // Set ellipse mask (cut to ellipse shape)
    if (shape == OutputShape::ELLIPSE) {
        QSize size(croppedWidth, croppedHeight);
        QBitmap mask(size);
        QPainter painter(&mask);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
        painter.setBrush(QColor(0, 0, 0));
        painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
        resultImage.setMask(mask);
    }

    return resultImage;
}


void ImageCropperLabel::paintEvent(QPaintEvent *event) {
    // Draw original image
    QLabel::paintEvent(event);

    // Draw cropper and set some effects
    switch (cropperShape) {
        case CropperShape::UNDEFINED:
            break;
        case CropperShape::FIXED_RECT:
            drawRectOpacity();
            break;
        case CropperShape::FIXED_ELLIPSE:
            drawEllipseOpacity();
            break;
        case CropperShape::RECT:
            drawRectOpacity();
            drawSquareEdge(!ONLY_FOUR_CORNERS);
            break;
        case CropperShape::SQUARE:
            drawRectOpacity();
            drawSquareEdge(ONLY_FOUR_CORNERS);
            break;
        case CropperShape::ELLIPSE:
            drawEllipseOpacity();
            drawSquareEdge(!ONLY_FOUR_CORNERS);
            break;
        case CropperShape::CIRCLE:
            drawEllipseOpacity();
            drawSquareEdge(ONLY_FOUR_CORNERS);
            break;
    }

    // Draw cropper rect
    if (isShowRectBorder) {
        QPainter painter(this);
        painter.setPen(borderPen);
        painter.drawRect(cropperRect);
    }
}

void ImageCropperLabel::drawSquareEdge(bool onlyFourCorners) {
    if (!isShowDragSquare)
        return;

    // Four corners
    drawFillRect(cropperRect.topLeft(), dragSquareEdge, dragSquareColor);
    drawFillRect(cropperRect.topRight(), dragSquareEdge, dragSquareColor);
    drawFillRect(cropperRect.bottomLeft(), dragSquareEdge, dragSquareColor);
    drawFillRect(cropperRect.bottomRight(), dragSquareEdge, dragSquareColor);

    // Four edges
    if (!onlyFourCorners) {
        int centralX = cropperRect.left() + cropperRect.width() / 2;
        int centralY = cropperRect.top() + cropperRect.height() / 2;
        drawFillRect(QPoint(cropperRect.left(), centralY), dragSquareEdge, dragSquareColor);
        drawFillRect(QPoint(centralX, cropperRect.top()), dragSquareEdge, dragSquareColor);
        drawFillRect(QPoint(cropperRect.right(), centralY), dragSquareEdge, dragSquareColor);
        drawFillRect(QPoint(centralX, cropperRect.bottom()), dragSquareEdge, dragSquareColor);
    }
}

void ImageCropperLabel::drawFillRect(QPoint centralPoint, int edge, QColor color) {
    QRect rect(centralPoint.x() - edge / 2, centralPoint.y() - edge / 2, edge, edge);
    QPainter painter(this);
    painter.fillRect(rect, color);
}

// Opacity effect
void ImageCropperLabel::drawOpacity(const QPainterPath& path) {
    QPainter painterOpac(this);
    painterOpac.setOpacity(opacity);
    painterOpac.fillPath(path, QBrush(Qt::black));
}

void ImageCropperLabel::drawRectOpacity() {
    if (isShowOpacityEffect) {
        QPainterPath p1, p2, p;
        p1.addRect(imageRect);
        p2.addRect(cropperRect);
        p = p1.subtracted(p2);
        drawOpacity(p);
    }
}

void ImageCropperLabel::drawEllipseOpacity() {
    if (isShowOpacityEffect) {
        QPainterPath p1, p2, p;
        p1.addRect(imageRect);
        p2.addEllipse(cropperRect);
        p = p1.subtracted(p2);
        drawOpacity(p);
    }
}

bool ImageCropperLabel::isPosNearDragSquare(const QPoint& pt1, const QPoint& pt2) {
    return abs(pt1.x() - pt2.x()) * 2 <= dragSquareEdge
           && abs(pt1.y() - pt2.y()) * 2 <= dragSquareEdge;
}

int ImageCropperLabel::getPosInCropperRect(const QPoint &pt) {
    if (isPosNearDragSquare(pt, QPoint(cropperRect.right(), cropperRect.center().y())))
        return RECT_RIGHT;
    if (isPosNearDragSquare(pt, cropperRect.bottomRight()))
        return RECT_BOTTOM_RIGHT;
    if (isPosNearDragSquare(pt, QPoint(cropperRect.center().x(), cropperRect.bottom())))
        return RECT_BOTTOM;
    if (isPosNearDragSquare(pt, cropperRect.bottomLeft()))
        return RECT_BOTTOM_LEFT;
    if (isPosNearDragSquare(pt, QPoint(cropperRect.left(), cropperRect.center().y())))
        return RECT_LEFT;
    if (isPosNearDragSquare(pt, cropperRect.topLeft()))
        return RECT_TOP_LEFT;
    if (isPosNearDragSquare(pt, QPoint(cropperRect.center().x(), cropperRect.top())))
        return RECT_TOP;
    if (isPosNearDragSquare(pt, cropperRect.topRight()))
        return RECT_TOP_RIGHT;
    if (cropperRect.contains(pt, true))
        return RECT_INSIDE;
    return RECT_OUTSIZD;
}

/*************************************************
 *
 *  Change mouse cursor type
 *      Arrow, SizeHor, SizeVer, etc...
 *
*************************************************/

void ImageCropperLabel::changeCursor() {
    switch (cursorPosInCropperRect) {
        case RECT_OUTSIZD:
            setCursor(Qt::ArrowCursor);
            break;
        case RECT_BOTTOM_RIGHT: {
            switch (cropperShape) {
            case CropperShape::SQUARE:
            case CropperShape::CIRCLE:
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeFDiagCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_RIGHT: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeHorCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_BOTTOM: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeVerCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_BOTTOM_LEFT: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
            case CropperShape::SQUARE:
            case CropperShape::CIRCLE:
                setCursor(Qt::SizeBDiagCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_LEFT: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeHorCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_TOP_LEFT: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
            case CropperShape::SQUARE:
            case CropperShape::CIRCLE:
                setCursor(Qt::SizeFDiagCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_TOP: {
            switch (cropperShape) {
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeVerCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_TOP_RIGHT: {
            switch (cropperShape) {
            case CropperShape::SQUARE:
            case CropperShape::CIRCLE:
            case CropperShape::RECT:
            case CropperShape::ELLIPSE:
                setCursor(Qt::SizeBDiagCursor);
                break;
            default:
                break;
            }
            break;
        }
        case RECT_INSIDE: {
            setCursor(Qt::SizeAllCursor);
            break;
        }
    }
}

/*****************************************************
 *
 *  Mouse Events
 *
*****************************************************/

void ImageCropperLabel::mousePressEvent(QMouseEvent *e) {
    currPos = lastPos = e->pos();
    isLButtonPressed = true;
}

void ImageCropperLabel::mouseMoveEvent(QMouseEvent *e) {
    currPos = e->pos();
    if (!isCursorPosCalculated) {
        cursorPosInCropperRect = getPosInCropperRect(currPos);
        changeCursor();
    }

    if (!isLButtonPressed)
        return;
    if (!imageRect.contains(currPos))
        return;

    isCursorPosCalculated = true;

    int xOffset = currPos.x() - lastPos.x();
    int yOffset = currPos.y() - lastPos.y();
    lastPos = currPos;

    int disX = 0;
    int disY = 0;

    // Move cropper
    switch (cursorPosInCropperRect) {
        case RECT_OUTSIZD:
            break;
        case RECT_BOTTOM_RIGHT: {
            disX = currPos.x() - cropperRect.left();
            disY = currPos.y() - cropperRect.top();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                    break;
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    setCursor(Qt::SizeFDiagCursor);
                    if (disX >= cropperMinimumWidth && disY >= cropperMinimumHeight) {
                        if (disX > disY && cropperRect.top() + disX <= imageRect.bottom()) {
                            cropperRect.setRight(currPos.x());
                            cropperRect.setBottom(cropperRect.top() + disX);
                            emit croppedImageChanged();
                        }
                        else if (disX <= disY && cropperRect.left() + disY <= imageRect.right()) {
                            cropperRect.setBottom(currPos.y());
                            cropperRect.setRight(cropperRect.left() + disY);
                            emit croppedImageChanged();
                        }
                    }
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    setCursor(Qt::SizeFDiagCursor);
                    if (disX >= cropperMinimumWidth) {
                        cropperRect.setRight(currPos.x());
                        emit croppedImageChanged();
                    }
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setBottom(currPos.y());
                        emit croppedImageChanged();
                    }
                    break;
            }
            break;
        }
        case RECT_RIGHT: {
            disX = currPos.x() - cropperRect.left();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disX >= cropperMinimumWidth) {
                        cropperRect.setRight(currPos.x());
                        emit croppedImageChanged();
                    }
                    break;
            }
            break;
        }
        case RECT_BOTTOM: {
            disY = currPos.y() - cropperRect.top();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setBottom(cropperRect.bottom() + yOffset);
                        emit croppedImageChanged();
                    }
                    break;
            }
            break;
        }
        case RECT_BOTTOM_LEFT: {
            disX = cropperRect.right() - currPos.x();
            disY = currPos.y() - cropperRect.top();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                    break;
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disX >= cropperMinimumWidth) {
                        cropperRect.setLeft(currPos.x());
                        emit croppedImageChanged();
                    }
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setBottom(currPos.y());
                        emit croppedImageChanged();
                    }
                    break;
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    if (disX >= cropperMinimumWidth && disY >= cropperMinimumHeight) {
                        if (disX > disY && cropperRect.top() + disX <= imageRect.bottom()) {
                            cropperRect.setLeft(currPos.x());
                            cropperRect.setBottom(cropperRect.top() + disX);
                            emit croppedImageChanged();
                        }
                        else if (disX <= disY && cropperRect.right() - disY >= imageRect.left()) {
                            cropperRect.setBottom(currPos.y());
                            cropperRect.setLeft(cropperRect.right() - disY);
                            emit croppedImageChanged();
                        }
                    }
                    break;
            }
            break;
        }
        case RECT_LEFT: {
            disX = cropperRect.right() - currPos.x();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disX >= cropperMinimumHeight) {
                        cropperRect.setLeft(cropperRect.left() + xOffset);
                        emit croppedImageChanged();
                    }
                    break;
            }
            break;
        }
        case RECT_TOP_LEFT: {
            disX = cropperRect.right() - currPos.x();
            disY = cropperRect.bottom() - currPos.y();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disX >= cropperMinimumWidth) {
                        cropperRect.setLeft(currPos.x());
                        emit croppedImageChanged();
                    }
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setTop(currPos.y());
                        emit croppedImageChanged();
                    }
                    break;
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    if (disX >= cropperMinimumWidth && disY >= cropperMinimumHeight) {
                        if (disX > disY && cropperRect.bottom() - disX >= imageRect.top()) {
                            cropperRect.setLeft(currPos.x());
                            cropperRect.setTop(cropperRect.bottom() - disX);
                            emit croppedImageChanged();
                        }
                        else if (disX <= disY && cropperRect.right() - disY >= imageRect.left()) {
                            cropperRect.setTop(currPos.y());
                            cropperRect.setLeft(cropperRect.right() - disY);
                            emit croppedImageChanged();
                        }
                    }
                    break;
            }
            break;
        }
        case RECT_TOP: {
            disY = cropperRect.bottom() - currPos.y();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setTop(cropperRect.top() + yOffset);
                        emit croppedImageChanged();
                    }
                    break;
            }
            break;
        }
        case RECT_TOP_RIGHT: {
            disX = currPos.x() - cropperRect.left();
            disY = cropperRect.bottom() - currPos.y();
            switch (cropperShape) {
                case CropperShape::UNDEFINED:
                case CropperShape::FIXED_RECT:
                case CropperShape::FIXED_ELLIPSE:
                    break;
                case CropperShape::RECT:
                case CropperShape::ELLIPSE:
                    if (disX >= cropperMinimumWidth) {
                        cropperRect.setRight(currPos.x());
                        emit croppedImageChanged();
                    }
                    if (disY >= cropperMinimumHeight) {
                        cropperRect.setTop(currPos.y());
                        emit croppedImageChanged();
                    }
                    break;
                case CropperShape::SQUARE:
                case CropperShape::CIRCLE:
                    if (disX >= cropperMinimumWidth && disY >= cropperMinimumHeight) {
                        if (disX < disY && cropperRect.left() + disY <= imageRect.right()) {
                            cropperRect.setTop(currPos.y());
                            cropperRect.setRight(cropperRect.left() + disY);
                            emit croppedImageChanged();
                        }
                        else if (disX >= disY && cropperRect.bottom() - disX >= imageRect.top()) {
                            cropperRect.setRight(currPos.x());
                            cropperRect.setTop(cropperRect.bottom() - disX);
                            emit croppedImageChanged();
                        }
                    }
                    break;
            }
            break;
        }
        case RECT_INSIDE: {
            // Make sure the cropperRect is entirely inside the imageRecct
            if (xOffset > 0) {
                if (cropperRect.right() + xOffset > imageRect.right())
                    xOffset = 0;
            }
            else if (xOffset < 0) {
                if (cropperRect.left() + xOffset < imageRect.left())
                    xOffset = 0;
            }
            if (yOffset > 0) {
                if (cropperRect.bottom() + yOffset > imageRect.bottom())
                    yOffset = 0;
            }
            else if (yOffset < 0) {
                if (cropperRect.top() + yOffset < imageRect.top())
                    yOffset = 0;
            }
            cropperRect.moveTo(cropperRect.left() + xOffset, cropperRect.top() + yOffset);
            emit croppedImageChanged();
        }
        break;
    }

    repaint();
}

void ImageCropperLabel::mouseReleaseEvent(QMouseEvent *) {
    isLButtonPressed = false;
    isCursorPosCalculated = false;
    setCursor(Qt::ArrowCursor);
}

