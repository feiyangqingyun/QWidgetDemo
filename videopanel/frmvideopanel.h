#ifndef FRMVIDEOPANEL_H
#define FRMVIDEOPANEL_H

#include <QWidget>

namespace Ui {
class frmVideoPanel;
}

class frmVideoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPanel(QWidget *parent = 0);
    ~frmVideoPanel();

private:
    Ui::frmVideoPanel *ui;
};

#endif // FRMVIDEOPANEL_H
