#ifndef FRMTRAYICON_H
#define FRMTRAYICON_H

#include <QWidget>

namespace Ui {
class frmTrayIcon;
}

class frmTrayIcon : public QWidget
{
    Q_OBJECT

public:
    explicit frmTrayIcon(QWidget *parent = 0);
    ~frmTrayIcon();

private:
    Ui::frmTrayIcon *ui;

private slots:
    void on_btnShow_clicked();
    void on_btnHide_clicked();
};

#endif // FRMTRAYICON_H
