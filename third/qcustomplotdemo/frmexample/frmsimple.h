#ifndef FRMSIMPLE_H
#define FRMSIMPLE_H

#include <QWidget>

namespace Ui {
class frmSimple;
}

class frmSimple : public QWidget
{
    Q_OBJECT

public:
    explicit frmSimple(QWidget *parent = 0);
    ~frmSimple();

private:
    Ui::frmSimple *ui;

private slots:
    void initForm();
};

#endif // FRMSIMPLE_H
