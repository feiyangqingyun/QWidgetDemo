#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelessmainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public FramelessMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void initForm();
    void titleDblClick();
    void windowStateChange(bool max);

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
};

#endif // MAINWINDOW_H
