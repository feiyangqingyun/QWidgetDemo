#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>

namespace Ui {
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

private slots:
    void initForm();
    void loadStyle(const QString &qssFile);

private slots:
    void on_btnStyle1_clicked();
    void on_btnStyle2_clicked();
    void on_btnStyle3_clicked();
    void on_btnInfo_clicked();
};

#endif // FRMMAIN_H
