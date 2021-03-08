#ifndef FRMCOLORWIDGET_H
#define FRMCOLORWIDGET_H

#include <QWidget>

namespace Ui {
class frmColorWidget;
}

class frmColorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmColorWidget(QWidget *parent = 0);
    ~frmColorWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::frmColorWidget *ui;
};

#endif // FRMCOLORWIDGET_H
