#ifndef FRMLIGHTBUTTON_H
#define FRMLIGHTBUTTON_H

#include <QWidget>

namespace Ui {
class frmLightButton;
}

class frmLightButton : public QWidget
{
    Q_OBJECT

public:
    explicit frmLightButton(QWidget *parent = 0);
    ~frmLightButton();

private:
    Ui::frmLightButton *ui;
    int type;

private slots:
    void initForm();
    void updateValue();
};

#endif // FRMLIGHTBUTTON_H
