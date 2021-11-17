#ifndef FRMSHORTCUT_H
#define FRMSHORTCUT_H

#include <QWidget>

namespace Ui {
class frmShortCut;
}

class frmShortCut : public QWidget
{
    Q_OBJECT

public:
    explicit frmShortCut(QWidget *parent = 0);
    ~frmShortCut();

private:
    Ui::frmShortCut *ui;

private slots:
    void initForm();
    void shortcut();
};

#endif // FRMSHORTCUT_H
