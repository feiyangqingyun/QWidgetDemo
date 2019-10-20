#ifndef FRMMASKWIDGET_H
#define FRMMASKWIDGET_H

#include <QWidget>

namespace Ui {
class frmMaskWidget;
}

class frmMaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmMaskWidget(QWidget *parent = 0);
    ~frmMaskWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::frmMaskWidget *ui;
};

#endif // FRMMASKWIDGET_H
