#ifndef FRMSCATTERPIXMAP_H
#define FRMSCATTERPIXMAP_H

#include <QWidget>

namespace Ui {
class frmScatterPixmap;
}

class frmScatterPixmap : public QWidget
{
    Q_OBJECT

public:
    explicit frmScatterPixmap(QWidget *parent = 0);
    ~frmScatterPixmap();

private:
    Ui::frmScatterPixmap *ui;

private slots:
    void initForm();
};

#endif // FRMSCATTERPIXMAP_H
