#ifndef FRMDEVICESIZETABLE_H
#define FRMDEVICESIZETABLE_H

#include <QWidget>

namespace Ui {
class frmDeviceSizeTable;
}

class frmDeviceSizeTable : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceSizeTable(QWidget *parent = 0);
    ~frmDeviceSizeTable();

private:
    Ui::frmDeviceSizeTable *ui;
};

#endif // FRMDEVICESIZETABLE_H
