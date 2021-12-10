#ifndef FRMLINESTYLE_H
#define FRMLINESTYLE_H

#include <QWidget>

namespace Ui {
class frmLineStyle;
}

class frmLineStyle : public QWidget
{
    Q_OBJECT

public:
    explicit frmLineStyle(QWidget *parent = 0);
    ~frmLineStyle();

private:
    Ui::frmLineStyle *ui;

private slots:
    void initForm();
};

#endif // FRMLINESTYLE_H
