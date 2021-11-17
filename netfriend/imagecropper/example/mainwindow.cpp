#include "mainwindow.h"
#include "imagecropperdemo.h"

#include "../base/imagecropperdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupLayout();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupLayout() {
    QWidget* centralWidget = new QWidget(this);

    btnCustomCrop = new QPushButton("Custom Crop", centralWidget);
    btnSimpleCrop = new QPushButton("Simple Crop", centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(btnCustomCrop);
    mainLayout->addWidget(btnSimpleCrop);
    this->setCentralWidget(centralWidget);

    connect(btnCustomCrop, &QPushButton::clicked, this, &MainWindow::onCustomCrop);
    connect(btnSimpleCrop, &QPushButton::clicked, this, &MainWindow::onSimpleCrop);
}

void MainWindow::onCustomCrop() {
    ImageCropperDemo* dialog = new ImageCropperDemo(this);
    dialog->show();
}

void MainWindow::onSimpleCrop() {
    QMessageBox::information(this, "Prompt", "Please select a picture", QMessageBox::Ok);
    QString filename = QFileDialog::getOpenFileName(this, "Select image", "", "image (*.png *.jpg)");
    if (filename.isNull())
        return;

    //                                   *********
    //                                    *******
    //                                     *****
    //                                      ***
    //                                       *
    QPixmap image = ImageCropperDialog::getCroppedImage(filename, 600, 400, CropperShape::CIRCLE);
    if (image.isNull())
        return;

    QDialog* dialog = new QDialog(0);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    QLabel* label = new QLabel(dialog);
    label->setFixedSize(image.size());
    label->setPixmap(image);
    dialog->exec();
}
