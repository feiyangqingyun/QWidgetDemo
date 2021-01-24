#ifndef FRMWEBSERVER_H
#define FRMWEBSERVER_H

#include <QWidget>
#include "webserver.h"

namespace Ui {
class frmWebServer;
}

class frmWebServer : public QWidget
{
    Q_OBJECT

public:
    explicit frmWebServer(QWidget *parent = 0);
    ~frmWebServer();

private:
    Ui::frmWebServer *ui;

    bool isOk;
    WebServer *server;
    QTimer *timer;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void changeTimer();
    void append(int type, const QString &data, bool clear = false);

private slots:
    void clientConnected(const QString &ip, int port);
    void clientDisconnected(const QString &ip, int port);
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

private slots:
    void on_btnListen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
    void on_btnClose_clicked();
};

#endif // FRMWEBSERVER_H
