#ifndef FRMZHTOPY_H
#define FRMZHTOPY_H

#include <QWidget>

namespace Ui {
class frmZhToPY;
}

class frmZhToPY : public QWidget
{
    Q_OBJECT

public:
    explicit frmZhToPY(QWidget *parent = 0);
    ~frmZhToPY();

private:
    Ui::frmZhToPY *ui;

private slots:
    void on_btnPY_clicked();
    void on_btnJP_clicked();
};

#endif // FRMZHTOPY_H
