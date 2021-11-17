#ifndef TESTIMAGECROPPERLABEL_H
#define TESTIMAGECROPPERLABEL_H

#include <QObject>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>

#include "../base/imagecropperlabel.h"

class ImageCropperDemo : public QDialog
{
    Q_OBJECT
public:
    ImageCropperDemo(QWidget* parent = 0);

    void setupLayout();

    void init();

public slots:
    void onOutputShapeChanged(int idx);
    void onCropperShapeChanged(int idx);
    void onEnableOpacityChanged(int state);
    void onShowDragSquareChanged(int state);
    void onDragSquareEdgeChanged(QString edge);
    void onOpacityChanged(int val);
    void onFixedWidthChanged(QString width);
    void onFixedHeightChanged(QString height);
    void onMinWidthChanged(QString width);
    void onMinHeightChanged(QString height);
    void onShowRectBorder(int state);
    void onChooseRectBorderColor();
    void onChooseDragSquareColor();

    void onChooseOriginalImage();
    void onUpdatePreview();
    void onSaveCroppedImage();

private:
    void setLabelColor(QLabel* label, QColor color) {
        QPixmap pixmap(QSize(80, 25));
        pixmap.fill(color);
        label->setPixmap(pixmap);
    }

private:
    ImageCropperLabel* imgCropperLabel;
    QHBoxLayout* mainLayout;

    QLabel* labelPreviewImage;

    QComboBox* comboOutputShape;
    QComboBox* comboCropperShape;

    QLineEdit* editOriginalImagePath;
    QPushButton* btnChooseOriginalImagePath;

    QLineEdit* editCropperFixedWidth;
    QLineEdit* editCropperFixedHeight;
    QLineEdit* editCropperMinWidth;
    QLineEdit* editCropperMinHeight;

    QCheckBox* checkShowDragSquare;
    QCheckBox* checkEnableOpacity;
    QSlider* sliderOpacity;
    QLineEdit* editDragSquareEdge;

    QCheckBox* checkShowRectBorder;
    QLabel* labelRectBorderColor;
    QPushButton* btnChooseRectBorderCorlor;

    QLabel* labelDragSquareColor;
    QPushButton* btnChooseDragSquareColor;

    QPushButton* btnSavePreview;
    QPushButton* btnQuit;
};

#endif // TESTIMAGECROPPERLABEL_H
