#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

private slots:
    void initForm();
    QString getWMIC(const QString &cmd);
    QString getCpuName();
    QString getCpuId();
    QString getDiskNum();
    QString getXorEncryptDecrypt(const QString &data, char key);

private slots:
    void on_btnOk_clicked();
    void on_btnClose_clicked();
};

#endif // FRMMAIN_H
