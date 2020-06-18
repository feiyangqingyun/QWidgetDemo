#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

class QUdpSocket;

namespace Ui
{
    class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::frmMain *ui;
    QTimer *timerHeart;     //心跳定时器
    QUdpSocket *udp;        //UDP通信对象
    int count;              //计数
    bool ok;                //是否正常

private slots:
    void initForm();
    void sendHearData();
    void readData();
    void killApp();
    void killOther();
    void startApp();
    void startExplorer();

private slots:
    void on_btnOk_clicked();
    void on_btnStart_clicked();
    void on_btnReset_clicked();
};

#endif // FRMMAIN_H
