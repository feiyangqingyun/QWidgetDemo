#ifndef FRMCOLORMAP_H
#define FRMCOLORMAP_H

#include <QWidget>

namespace Ui {
class frmColorMap;
}

class frmColorMap : public QWidget
{
    Q_OBJECT

public:
    explicit frmColorMap(QWidget *parent = 0);
    ~frmColorMap();

private:
    Ui::frmColorMap *ui;

private slots:
    void initForm();
};

#endif // FRMCOLORMAP_H
