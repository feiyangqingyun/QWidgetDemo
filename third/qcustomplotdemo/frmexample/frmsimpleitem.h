#ifndef FRMSIMPLEITEM_H
#define FRMSIMPLEITEM_H

#include <QWidget>

namespace Ui {
class frmSimpleItem;
}

class frmSimpleItem : public QWidget
{
    Q_OBJECT

public:
    explicit frmSimpleItem(QWidget *parent = 0);
    ~frmSimpleItem();

private:
    Ui::frmSimpleItem *ui;

private slots:
    void initForm();
};

#endif // FRMSIMPLEITEM_H
