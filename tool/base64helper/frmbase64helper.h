#ifndef FRMBASE64HELPER_H
#define FRMBASE64HELPER_H

#include <QWidget>

namespace Ui {
class frmBase64Helper;
}

class frmBase64Helper : public QWidget
{
    Q_OBJECT

public:
    explicit frmBase64Helper(QWidget *parent = 0);
    ~frmBase64Helper();

private:
    Ui::frmBase64Helper *ui;

private slots:
    void on_btnOpen_clicked();
    void on_btnClear_clicked();
    void on_btnImageToBase64_clicked();
    void on_btnBase64ToImage_clicked();
    void on_btnTextToBase64_clicked();
    void on_btnBase64ToText_clicked();
};

#endif // FRMBASE64HELPER_H
