#ifndef FRMVIDEOWINDOW_H
#define FRMVIDEOWINDOW_H

#include <QWidget>

namespace Ui {
class frmVideoWindow;
}

class frmVideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoWindow(QWidget *parent = 0);
    ~frmVideoWindow();

private:
    Ui::frmVideoWindow *ui;

private slots:
    void initForm();
    void btnClicked(const QString &objName);
};

#endif // FRMVIDEOWINDOW_H
