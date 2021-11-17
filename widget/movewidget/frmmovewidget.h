#ifndef FRMMOVEWIDGET_H
#define FRMMOVEWIDGET_H

#include <QWidget>

namespace Ui {
class frmMoveWidget;
}

class frmMoveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmMoveWidget(QWidget *parent = 0);
    ~frmMoveWidget();

private:
    Ui::frmMoveWidget *ui;

private slots:
    void initForm();
};

#endif // FRMMOVEWIDGET_H
