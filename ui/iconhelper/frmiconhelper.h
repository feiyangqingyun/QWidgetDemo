#ifndef FRMICONHELPER_H
#define FRMICONHELPER_H

#include <QWidget>
class QButtonGroup;

namespace Ui {
class frmIconHelper;
}

class frmIconHelper : public QWidget
{
    Q_OBJECT

public:
    explicit frmIconHelper(QWidget *parent = 0);
    ~frmIconHelper();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmIconHelper *ui;
    int iconSize;
    QList<int> icons;
    QList<QWidget *> labs;

private slots:
    void initForm();
    void initPanel();
    bool checkIcon(int icon);
    void toggled(bool checked);

    void initBtn(QButtonGroup *btnGroup, bool textBesideIcon);
    void btnClicked();

    void initWidget1();
    void initWidget2();
    void initWidget3();
    void initWidget4();
    void initWidget5();
    void initWidget6();
};

#endif // FRMICONHELPER_H
