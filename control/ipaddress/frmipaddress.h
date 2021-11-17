#ifndef FRMADDRESS_H
#define FRMADDRESS_H

#include <QWidget>

namespace Ui {
class frmIPAddress;
}

class frmIPAddress : public QWidget
{
    Q_OBJECT

public:
    explicit frmIPAddress(QWidget *parent = 0);
    ~frmIPAddress();

private:
    Ui::frmIPAddress *ui;

private slots:
    void on_btnSetIP_clicked();
    void on_btnGetIP_clicked();
    void on_btnClear_clicked();
};

#endif // FRMADDRESS_H
