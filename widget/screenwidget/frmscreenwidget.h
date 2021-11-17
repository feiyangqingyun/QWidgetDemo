#ifndef FRMSCREENWIDGET_H
#define FRMSCREENWIDGET_H

#include <QWidget>

namespace Ui {
class frmScreenWidget;
}

class frmScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmScreenWidget(QWidget *parent = 0);
    ~frmScreenWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::frmScreenWidget *ui;
};

#endif // FRMSCREENWIDGET_H
