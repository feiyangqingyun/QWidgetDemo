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

private:
    Ui::frmMaskWidget *ui;

private slots:
    void initForm();
    void on_pushButton_clicked();
};

#endif // FRMMASKWIDGET_H
