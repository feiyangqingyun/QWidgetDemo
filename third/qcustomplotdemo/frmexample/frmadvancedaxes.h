#ifndef FRMADVANCEDAXES_H
#define FRMADVANCEDAXES_H

#include <QWidget>

namespace Ui {
class frmAdvancedAxes;
}

class frmAdvancedAxes : public QWidget
{
    Q_OBJECT

public:
    explicit frmAdvancedAxes(QWidget *parent = 0);
    ~frmAdvancedAxes();

private:
    Ui::frmAdvancedAxes *ui;

private slots:
    void initForm();
};

#endif // FRMADVANCEDAXES_H
