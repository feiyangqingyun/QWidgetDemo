#ifndef FRMFRAMELESSWIDGET_H
#define FRMFRAMELESSWIDGET_H

#include <QWidget>

namespace Ui
{
    class frmFramelessWidget;
}

class frmFramelessWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmFramelessWidget(QWidget *parent = 0);
    ~frmFramelessWidget();

private:
    Ui::frmFramelessWidget *ui;

private slots:
    void on_btnOpen_clicked();
};

#endif // FRMFRAMELESSWIDGET_H
