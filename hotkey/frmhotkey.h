#ifndef FRMHOTKEY_H
#define FRMHOTKEY_H

#include <QWidget>

namespace Ui {
class frmHotKey;
}

class frmHotKey : public QWidget
{
    Q_OBJECT

public:
    explicit frmHotKey(QWidget *parent = 0);
    ~frmHotKey();

private:
    Ui::frmHotKey *ui;

private slots:
    void initForm();
    void shortcut();
};

#endif // FRMHOTKEY_H
