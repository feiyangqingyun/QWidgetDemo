#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include "tcpserver1.h"
#include "tcpserver2.h"

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

private:
    Ui::frmMain *ui;
    TcpServer1 *tcpServer1;
    TcpServer2 *tcpServer2;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void append1(int type, const QString &data, bool clear = false);
    void append2(int type, const QString &data, bool clear = false);

private slots:
    void clientConnected1(const QString &ip, int port, const QString &deviceID);
    void clientDisconnected1(const QString &ip, int port, const QString &deviceID);
    void sendData1(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveData1(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveDeviceID1(const QString &ip, int port, const QString &deviceID);

    void clientConnected2(const QString &ip, int port, const QString &deviceID);
    void clientDisconnected2(const QString &ip, int port, const QString &deviceID);
    void sendData2(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveData2(const QString &ip, int port, const QString &deviceID, const QString &data);
    void receiveDeviceID2(const QString &ip, int port, const QString &deviceID);

private slots:
    void on_btnListen1_clicked();
    void on_btnClear1_clicked();
    void on_btnListen2_clicked();
    void on_btnClear2_clicked();
};

#endif // FRMMAIN_H
