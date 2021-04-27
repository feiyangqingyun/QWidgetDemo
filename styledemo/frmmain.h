#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>

namespace Ui
{
class frmMain;
}

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmMain *ui;

private slots:
    void initForm();
    void initTableWidget();
    void initTreeWidget();
    void initListWidget();
    void initOther();
    void initStyle();
    void initTranslator();
};

#endif // FRMMAIN_H
