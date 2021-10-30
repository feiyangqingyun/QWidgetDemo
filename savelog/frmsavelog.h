#ifndef FRMSAVELOG_H
#define FRMSAVELOG_H

#include <QWidget>
#include <QListWidgetItem>

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
    int count;
    QTimer *timer;

private slots:
    void initForm();
    void append(const QString &flag = QString());

private slots:
    void on_btnLog_clicked();
    void on_ckTimer_stateChanged(int arg1);
    void on_ckNet_stateChanged(int arg1);
    void on_ckSave_stateChanged(int arg1);

    void on_cboxSize_currentIndexChanged(int index);
    void on_cboxRow_currentIndexChanged(int index);
    void on_listType_itemPressed(QListWidgetItem *item);
};

#endif // FRMSAVELOG_H
