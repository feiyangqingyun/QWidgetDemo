#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onCustomCrop();
    void onSimpleCrop();

private:
    void setupLayout();

private:
    QPushButton* btnCustomCrop;
    QPushButton* btnSimpleCrop;
};
#endif // MAINWINDOW_H
