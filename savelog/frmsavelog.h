#ifndef FRMSAVELOG_H
#define FRMSAVELOG_H

#include <QWidget>

namespace Ui {
class frmSaveLog;
}

class frmSaveLog : public QWidget
{
    Q_OBJECT

public:
    explicit frmSaveLog(QWidget *parent = 0);
    ~frmSaveLog();

private:
    Ui::frmSaveLog *ui;
    QTimer *timer;

private slots:
    void initForm();
    void append();
    void on_btnDebug_clicked();
    void on_ckTimer_stateChanged(int arg1);
    void on_ckNet_stateChanged(int arg1);
    void on_ckSave_stateChanged(int arg1);    
};

#endif // FRMSAVELOG_H
