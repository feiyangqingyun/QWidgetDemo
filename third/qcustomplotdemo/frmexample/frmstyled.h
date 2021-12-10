#ifndef FRMSTYLED_H
#define FRMSTYLED_H

#include <QWidget>

namespace Ui {
class frmStyled;
}

class frmStyled : public QWidget
{
    Q_OBJECT

public:
    explicit frmStyled(QWidget *parent = 0);
    ~frmStyled();

private:
    Ui::frmStyled *ui;

private slots:
    void initForm();
};

#endif // FRMSTYLED_H
