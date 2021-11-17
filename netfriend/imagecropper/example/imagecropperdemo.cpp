#include "imagecropperdemo.h"
#include <QFormLayout>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

ImageCropperDemo::ImageCropperDemo(QWidget* parent) :
    QDialog(parent)
{
    setupLayout();
    init();

    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle("Image Cropper Demo");
}


void ImageCropperDemo::setupLayout() {
    imgCropperLabel = new ImageCropperLabel(600, 500, this);
    imgCropperLabel->setFrameStyle(1);

    comboOutputShape = new QComboBox(this);
    comboCropperShape = new QComboBox(this);

    labelPreviewImage = new QLabel(this);

    editOriginalImagePath = new QLineEdit(this);
    btnChooseOriginalImagePath = new QPushButton(this);
    QHBoxLayout* hOriginalImagePathLayout = new QHBoxLayout();
    hOriginalImagePathLayout->addWidget(editOriginalImagePath);
    hOriginalImagePathLayout->addWidget(btnChooseOriginalImagePath);

    editCropperFixedWidth = new QLineEdit(this);
    editCropperFixedHeight = new QLineEdit(this);
    QHBoxLayout* hCropperFixedSizeLayout = new QHBoxLayout();
    hCropperFixedSizeLayout->addWidget(editCropperFixedWidth);
    hCropperFixedSizeLayout->addWidget(editCropperFixedHeight);

    editCropperMinWidth = new QLineEdit("8", this);
    editCropperMinHeight = new QLineEdit("8", this);
    QHBoxLayout* hCropperMinSizeLayout = new QHBoxLayout();
    hCropperMinSizeLayout->addWidget(editCropperMinWidth);
    hCropperMinSizeLayout->addWidget(editCropperMinHeight);

    checkEnableOpacity = new QCheckBox(this);
    sliderOpacity = new QSlider(Qt::Horizontal, this);

    checkShowDragSquare = new QCheckBox(this);
    editDragSquareEdge = new QLineEdit("8", this);
    checkShowRectBorder = new QCheckBox(this);

    labelRectBorderColor = new QLabel(this);
    btnChooseRectBorderCorlor = new QPushButton(this);
    QHBoxLayout* hRectBorderColorLayout = new QHBoxLayout();
    hRectBorderColorLayout->addWidget(labelRectBorderColor);
    hRectBorderColorLayout->addWidget(btnChooseRectBorderCorlor);

    labelDragSquareColor = new QLabel(this);
    btnChooseDragSquareColor = new QPushButton(this);
    QHBoxLayout* hDragSquareColorLayout = new QHBoxLayout();
    hDragSquareColorLayout->addWidget(labelDragSquareColor);
    hDragSquareColorLayout->addWidget(btnChooseDragSquareColor);

    QFormLayout* formLayout1 = new QFormLayout();
    formLayout1->addRow(new QLabel("Preview:"), labelPreviewImage);
    formLayout1->addRow(new QLabel("OriginalImage:", this), hOriginalImagePathLayout);
    formLayout1->addRow(new QLabel("OutputShape:", this), comboOutputShape);
    formLayout1->addRow(new QLabel("CropperShape:", this), comboCropperShape);
    formLayout1->addRow(new QLabel("FixedSize:", this), hCropperFixedSizeLayout);
    formLayout1->addRow(new QLabel("MinimumSize:", this), hCropperMinSizeLayout);

    QFormLayout* formLayout2 = new QFormLayout();
    formLayout2->addRow(new QLabel("EnableOpacity:", this), checkEnableOpacity);
    formLayout2->addRow(new QLabel("Opacity:", this), sliderOpacity);

    QFormLayout* formLayout3 = new QFormLayout();
    formLayout3->addRow(new QLabel("ShowDragSquare:", this), checkShowDragSquare);
    formLayout3->addRow(new QLabel("DragSquareEdge:", this), editDragSquareEdge);
    formLayout3->addRow(new QLabel("DragSquareColor:", this), hDragSquareColorLayout);

    QFormLayout* formLayout4 = new QFormLayout();
    formLayout4->addRow(new QLabel("ShowRectBorder:", this), checkShowRectBorder);
    formLayout4->addRow(new QLabel("RectBorderColor:", this), hRectBorderColorLayout);

    btnSavePreview = new QPushButton("Save", this);
    btnQuit = new QPushButton("Quit", this);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnSavePreview);
    btnLayout->addStretch();
    btnLayout->addWidget(btnQuit);
    btnLayout->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(formLayout1);
    vLayout->addStretch();
    vLayout->addLayout(formLayout2);
    vLayout->addStretch();
    vLayout->addLayout(formLayout3);
    vLayout->addStretch();
    vLayout->addLayout(formLayout4);
    vLayout->addStretch();
    vLayout->addLayout(btnLayout);

    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(imgCropperLabel);
    mainLayout->addLayout(vLayout);
}

void ImageCropperDemo::init() {
    imgCropperLabel->setRectCropper();
    editCropperFixedWidth->setEnabled(false);
    editCropperFixedHeight->setEnabled(false);

    labelPreviewImage->setFixedSize(96, 96);
    labelPreviewImage->setAlignment(Qt::AlignCenter);
    labelPreviewImage->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    connect(imgCropperLabel, &ImageCropperLabel::croppedImageChanged,
            this, &ImageCropperDemo::onUpdatePreview);

    btnChooseOriginalImagePath->setIcon(QIcon("res/select-file.ico"));
    btnChooseOriginalImagePath->setFixedWidth(30);
    connect(btnChooseOriginalImagePath, &QPushButton::clicked,
            this, &ImageCropperDemo::onChooseOriginalImage);

    comboOutputShape->addItem("Rect/Square");
    comboOutputShape->addItem("Ellipse/Circle");
    connect(comboOutputShape, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onOutputShapeChanged(int)));

    comboCropperShape->addItem("Rect");
    comboCropperShape->addItem("Square");
    comboCropperShape->addItem("FixedRect");
    comboCropperShape->addItem("Ellipse");
    comboCropperShape->addItem("Circle");
    comboCropperShape->addItem("FixedEllipse");
    connect(comboCropperShape, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onCropperShapeChanged(int)));

    connect(editCropperFixedWidth, &QLineEdit::textChanged,
            this, &ImageCropperDemo::onFixedWidthChanged);
    connect(editCropperFixedHeight, &QLineEdit::textChanged,
            this, &ImageCropperDemo::onFixedHeightChanged);
    connect(editCropperMinWidth, &QLineEdit::textChanged,
            this, &ImageCropperDemo::onMinWidthChanged);
    connect(editCropperMinHeight, &QLineEdit::textChanged,
            this, &ImageCropperDemo::onMinHeightChanged);

    checkEnableOpacity->setCheckState(Qt::Checked);
    imgCropperLabel->enableOpacity(true);
    connect(checkEnableOpacity, &QCheckBox::stateChanged,
            this, &ImageCropperDemo::onEnableOpacityChanged);

    checkShowDragSquare->setCheckState(Qt::Checked);
    imgCropperLabel->setShowDragSquare(true);
    connect(checkShowDragSquare, &QCheckBox::stateChanged,
            this, &ImageCropperDemo::onShowDragSquareChanged);
    connect(editDragSquareEdge, &QLineEdit::textChanged,
            this, &ImageCropperDemo::onDragSquareEdgeChanged);

    sliderOpacity->setRange(0, 100);
    sliderOpacity->setValue(60);
    connect(sliderOpacity, &QSlider::valueChanged,
            this, &ImageCropperDemo::onOpacityChanged);

    checkShowRectBorder->setCheckState(Qt::Checked);
    connect(checkShowRectBorder, &QCheckBox::stateChanged,
            this, &ImageCropperDemo::onShowRectBorder);

    setLabelColor(labelRectBorderColor, Qt::white);
    btnChooseRectBorderCorlor->setIcon(QIcon("res/color-palette.ico"));
    btnChooseRectBorderCorlor->setFixedWidth(40);
    connect(btnChooseRectBorderCorlor, &QPushButton::clicked,
            this, &ImageCropperDemo::onChooseRectBorderColor);

    setLabelColor(labelDragSquareColor, Qt::white);
    btnChooseDragSquareColor->setIcon(QIcon("res/color-palette.ico"));
    btnChooseDragSquareColor->setFixedWidth(40);
    connect(btnChooseDragSquareColor, &QPushButton::clicked,
            this, &ImageCropperDemo::onChooseDragSquareColor);

    connect(btnSavePreview, &QPushButton::clicked,
            this, &ImageCropperDemo::onSaveCroppedImage);
    connect(btnQuit, &QPushButton::clicked,
            this, &ImageCropperDemo::close);

    imgCropperLabel->update();
}


/*****************************************************************************
 *
 *    slots
 *
*****************************************************************************/

void ImageCropperDemo::onChooseOriginalImage() {
    QString filename = QFileDialog::getOpenFileName(this, "Select a picture", "",
                                                    "picture (*.jpg *.png *.bmp)");
    if (filename.isNull())
        return;

    QPixmap pixmap;
    if (!pixmap.load(filename)) {
        QMessageBox::critical(this, "Error", "Load image failed", QMessageBox::Ok);
        return;
    }

    editOriginalImagePath->setText(filename);
    imgCropperLabel->setOriginalImage(pixmap);
    imgCropperLabel->update();
    onUpdatePreview();
    labelPreviewImage->setFrameStyle(0);
}

void ImageCropperDemo::onOutputShapeChanged(int idx) {
    // Output: Rectangular
    if (idx == 0)
        imgCropperLabel->setOutputShape(OutputShape::RECT);
    else
        imgCropperLabel->setOutputShape(OutputShape::ELLIPSE);
    onUpdatePreview();
}

void ImageCropperDemo::onCropperShapeChanged(int idx) {
    switch (CropperShape(idx + 1)) {
    case CropperShape::RECT: {
        imgCropperLabel->setRectCropper();
        editCropperFixedWidth->setEnabled(false);
        editCropperFixedHeight->setEnabled(false);
        editCropperMinWidth->setEnabled(true);
        editCropperMinHeight->setEnabled(true);
        checkShowDragSquare->setEnabled(true);
        editDragSquareEdge->setEnabled(true);
        btnChooseDragSquareColor->setEnabled(true);
        break;
    }
    case CropperShape::SQUARE: {
        imgCropperLabel->setSquareCropper();
        editCropperFixedWidth->setEnabled(false);
        editCropperFixedHeight->setEnabled(false);
        editCropperMinWidth->setEnabled(true);
        editCropperMinHeight->setEnabled(true);
        checkShowDragSquare->setEnabled(true);
        editDragSquareEdge->setEnabled(true);
        btnChooseDragSquareColor->setEnabled(true);
        break;
    }
    case CropperShape::FIXED_RECT: {
        imgCropperLabel->setFixedRectCropper(QSize(64, 64));
        editCropperFixedWidth->setEnabled(true);
        editCropperFixedHeight->setEnabled(true);
        editCropperMinWidth->setEnabled(false);
        editCropperMinHeight->setEnabled(false);
        editCropperFixedWidth->setText("64");
        editCropperFixedHeight->setText("64");
        checkShowDragSquare->setEnabled(false);
        editDragSquareEdge->setEnabled(false);
        btnChooseDragSquareColor->setEnabled(false);
        break;
    }
    case CropperShape::ELLIPSE: {
        imgCropperLabel->setEllipseCropper();
        editCropperFixedWidth->setEnabled(false);
        editCropperFixedHeight->setEnabled(false);
        editCropperMinWidth->setEnabled(true);
        editCropperMinHeight->setEnabled(true);
        checkShowDragSquare->setEnabled(true);
        editDragSquareEdge->setEnabled(true);
        btnChooseDragSquareColor->setEnabled(true);
        break;
    }
    case CropperShape::CIRCLE: {
        imgCropperLabel->setCircleCropper();
        editCropperFixedWidth->setEnabled(false);
        editCropperFixedHeight->setEnabled(false);
        editCropperMinWidth->setEnabled(true);
        editCropperMinHeight->setEnabled(true);
        checkShowDragSquare->setEnabled(true);
        editDragSquareEdge->setEnabled(true);
        btnChooseDragSquareColor->setEnabled(true);
        break;
    }
    case CropperShape::FIXED_ELLIPSE:
        imgCropperLabel->setFixedEllipseCropper(QSize(64, 64));
        editCropperFixedWidth->setEnabled(true);
        editCropperFixedHeight->setEnabled(true);
        editCropperMinWidth->setEnabled(false);
        editCropperMinHeight->setEnabled(false);
        editCropperFixedWidth->setText("64");
        editCropperFixedHeight->setText("64");
        checkShowDragSquare->setEnabled(false);
        editDragSquareEdge->setEnabled(false);
        btnChooseDragSquareColor->setEnabled(false);
        break;
    case CropperShape::UNDEFINED:
        break;
    }

    imgCropperLabel->update();
    onUpdatePreview();
}

void ImageCropperDemo::onEnableOpacityChanged(int state) {
    if (state == Qt::Checked) {
        sliderOpacity->setEnabled(true);
        imgCropperLabel->enableOpacity(true);
    }
    else {
        sliderOpacity->setEnabled(false);
        imgCropperLabel->enableOpacity(false);
    }
    imgCropperLabel->update();
}

void ImageCropperDemo::onShowDragSquareChanged(int state) {
    if (state == Qt::Checked) {
        editDragSquareEdge->setEnabled(true);
        btnChooseDragSquareColor->setEnabled(true);
        imgCropperLabel->setShowDragSquare(true);
    }
    else {
        editDragSquareEdge->setEnabled(false);
        btnChooseDragSquareColor->setEnabled(false);
        imgCropperLabel->setShowDragSquare(false);
    }
    imgCropperLabel->update();
}

void ImageCropperDemo::onDragSquareEdgeChanged(QString edge) {
    imgCropperLabel->setDragSquareEdge(edge.toInt());
    imgCropperLabel->update();
}

void ImageCropperDemo::onOpacityChanged(int val) {
    imgCropperLabel->setOpacity(val / 100.0);
    imgCropperLabel->update();
}

void ImageCropperDemo::onFixedWidthChanged(QString width) {
    imgCropperLabel->setCropperFixedWidth(width.toInt());
    imgCropperLabel->update();
}

void ImageCropperDemo::onFixedHeightChanged(QString height) {
    imgCropperLabel->setCropperFixedHeight(height.toInt());
    imgCropperLabel->update();
}

void ImageCropperDemo::onMinWidthChanged(QString width) {
    imgCropperLabel->setCropperMinimumWidth(width.toInt());
    imgCropperLabel->update();
}

void ImageCropperDemo::onMinHeightChanged(QString height) {
    imgCropperLabel->setMinimumHeight(height.toInt());
    imgCropperLabel->update();
}

void ImageCropperDemo::onShowRectBorder(int state) {
    if (state == Qt::Checked) {
        btnChooseRectBorderCorlor->setEnabled(true);
        imgCropperLabel->setShowRectBorder(true);
    }
    else {
        btnChooseRectBorderCorlor->setEnabled(false);
        imgCropperLabel->setShowRectBorder(false);
    }
    imgCropperLabel->update();
}

void ImageCropperDemo::onChooseRectBorderColor() {
    QColor color = QColorDialog::getColor(imgCropperLabel->getBorderPen().color(), this);
    if (color.isValid()) {
        setLabelColor(labelRectBorderColor, color);
        QPen pen = imgCropperLabel->getBorderPen();
        pen.setColor(color);
        imgCropperLabel->setBorderPen(pen);
        imgCropperLabel->update();
    }
}

void ImageCropperDemo::onChooseDragSquareColor() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        setLabelColor(labelDragSquareColor, color);
        imgCropperLabel->setDragSquareColor(color);
        imgCropperLabel->update();
    }
}

void ImageCropperDemo::onUpdatePreview() {
    QPixmap preview = imgCropperLabel->getCroppedImage();
    preview = preview.scaled(labelPreviewImage->width(), labelPreviewImage->height(),
                             Qt::KeepAspectRatio, Qt::SmoothTransformation);
    labelPreviewImage->setPixmap(preview);
}

void ImageCropperDemo::onSaveCroppedImage() {
    const QPixmap* pixmap = labelPreviewImage->pixmap();
    if (!pixmap) {
        QMessageBox::information(this, "Error", "There is no cropped image to save.", QMessageBox::Ok);
        return ;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save cropped image", "", "picture (*.png)");
    if (!filename.isNull()) {
        if (imgCropperLabel->getCroppedImage().save(filename, "PNG"))
            QMessageBox::information(this, "Prompt", "Saved successfully", QMessageBox::Ok);
        else
            QMessageBox::information(this, "Error", "Save image failed!", QMessageBox::Ok);
    }
}

