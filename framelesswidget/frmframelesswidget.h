#ifndef FRMFRAMELESSWIDGET_H
#define FRMFRAMELESSWIDGET_H

#include <QWidget>
class FramelessWidget;

namespace Ui {
class frmFramelessWidget;
}

class frmFramelessWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmFramelessWidget(QWidget *parent = 0);
    ~frmFramelessWidget();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::frmFramelessWidget *ui;
    QWidget *w;
    FramelessWidget *frameless;

private slots:
    void on_pushButton_clicked();
    void stateChanged1(int arg1);
    void stateChanged2(int arg1);
};

#endif // FRMFRAMELESSWIDGET_H
