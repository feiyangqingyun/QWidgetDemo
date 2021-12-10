#ifndef FRMSCATTERSTYLE_H
#define FRMSCATTERSTYLE_H

#include <QWidget>

namespace Ui {
class frmScatterStyle;
}

class frmScatterStyle : public QWidget
{
    Q_OBJECT

public:
    explicit frmScatterStyle(QWidget *parent = 0);
    ~frmScatterStyle();

private:
    Ui::frmScatterStyle *ui;

private slots:
    void initForm();
};

#endif // FRMSCATTERSTYLE_H
