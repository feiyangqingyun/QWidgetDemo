#ifndef FRMNAVBUTTON_H
#define FRMNAVBUTTON_H

#include <QWidget>

class NavButton;

namespace Ui {
class frmNavButton;
}

class frmNavButton : public QWidget
{
    Q_OBJECT

public:
    explicit frmNavButton(QWidget *parent = 0);
    ~frmNavButton();

private:
    Ui::frmNavButton *ui;
    QList<int> icons;
    QList<NavButton *> btns1;
    QList<NavButton *> btns2;
    QList<NavButton *> btns3;
    QList<NavButton *> btns4;
    QList<NavButton *> btns5;
    QList<NavButton *> btns6;
    QList<NavButton *> btns7;

private slots:
    void initForm();
    void initBtn1();
    void initBtn2();
    void initBtn3();
    void initBtn4();
    void initBtn5();
    void initBtn6();
    void initBtn7();

    void buttonClick1();
    void buttonClick2();
    void buttonClick3();
    void buttonClick4();
    void buttonClick5();
    void buttonClick6();
    void buttonClick7();
};

#endif // FRMNAVBUTTON_H
