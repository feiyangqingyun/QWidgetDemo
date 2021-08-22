#ifndef FRMUDPCLIENT_H
#define FRMUDPCLIENT_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class frmUdpClient;
}

class frmUdpClient : public QWidget
{
    Q_OBJECT

public:
    explicit frmUdpClient(QWidget *parent = 0);
    ~frmUdpClient();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmUdpClient *ui;

    QUdpSocket *socket;
    QTimer *timer;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void initTimer();
    void append(int type, const QString &data, bool clear = false);

private slots:
    void error();
    void readData();
    void sendData(const QString &ip, int port, const QString &data);

private slots:
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
};

#endif // FRMUDPCLIENT_H
