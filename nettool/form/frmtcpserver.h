#ifndef FRMTCPSERVER_H
#define FRMTCPSERVER_H

#include <QWidget>
#include "tcpserver.h"

namespace Ui {
class frmTcpServer;
}

class frmTcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit frmTcpServer(QWidget *parent = 0);
    ~frmTcpServer();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmTcpServer *ui;

    bool isOk;
    TcpServer *server;
    QTimer *timer;

private slots:
    void initForm();    
    void initConfig();
    void saveConfig();
    void initTimer();
    void append(int type, const QString &data, bool clear = false);

private slots:
    void connected(const QString &ip, int port);
    void disconnected(const QString &ip, int port);
    void error(const QString &ip, int port, const QString &error);

    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

private slots:
    void on_btnListen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
    void on_btnRemove_clicked();
};

#endif // FRMTCPSERVER_H
