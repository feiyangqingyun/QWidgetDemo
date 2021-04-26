#ifndef FRMFLATUI_H
#define FRMFLATUI_H

#include <QWidget>

namespace Ui {
class frmFlatUI;
}

class frmFlatUI : public QWidget
{
    Q_OBJECT

public:
    explicit frmFlatUI(QWidget *parent = 0);
    ~frmFlatUI();

private:
    Ui::frmFlatUI *ui;

private slots:
    void initForm();
};

#endif // FRMFLATUI_H
