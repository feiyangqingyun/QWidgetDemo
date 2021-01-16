#ifndef FRMBUTTONDEFENCE_H
#define FRMBUTTONDEFENCE_H

#include <QWidget>

class ButtonDefence;
class QPushButton;

namespace Ui
{
    class frmButtonDefence;
}

class frmButtonDefence : public QWidget
{
    Q_OBJECT

public:
    explicit frmButtonDefence(QWidget *parent = 0);
    ~frmButtonDefence();

private slots:
    void initForm();
    void changeStyle();
    void changeStatus();
    void on_ckCanMove_stateChanged(int arg1);

private:
    Ui::frmButtonDefence *ui;
    ButtonDefence *btn1;
    ButtonDefence *btn2;
    ButtonDefence *btn3;
    QList<QPushButton *> btnStyle;
    QList<QPushButton *> btnStatus;
};

#endif // FRMBUTTONDEFENCE_H
