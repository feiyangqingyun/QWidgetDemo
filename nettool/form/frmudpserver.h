#ifndef FRMUDPSERVER_H
#define FRMUDPSERVER_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class frmUdpServer;
}

class frmUdpServer : public QWidget
{
    Q_OBJECT

public:
    explicit frmUdpServer(QWidget *parent = 0);
    ~frmUdpServer();

private:
    Ui::frmUdpServer *ui;    

    QUdpSocket *udpSocket;
    QTimer *timer;

private slots:
    void initForm();
    void initIP();
    void initConfig();
    void saveConfig();
    void changeTimer();
    void append(int type, const QString &data, bool clear = false);

    void readData();
    void sendData(const QString &ip, int port, const QString &data);
    void clientConnected(const QString &ip, int port);

private slots:
    void on_btnListen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
};

#endif // FRMUDPSERVER_H
