#ifndef FRMGIFWIDGET_H
#define FRMGIFWIDGET_H

#include <QWidget>

namespace Ui {
class frmGifWidget;
}

class frmGifWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmGifWidget(QWidget *parent = 0);
    ~frmGifWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::frmGifWidget *ui;
};

#endif // FRMGIFWIDGET_H
