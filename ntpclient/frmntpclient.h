#ifndef FRMNTPCLIENT_H
#define FRMNTPCLIENT_H

#include <QWidget>
#include <QDateTime>

namespace Ui {
class frmNtpClient;
}

class frmNtpClient : public QWidget
{
    Q_OBJECT

public:
    explicit frmNtpClient(QWidget *parent = 0);
    ~frmNtpClient();

private:
    Ui::frmNtpClient *ui;

private slots:
    void on_btnGetTime_clicked();
    void receiveTime(const QDateTime &dateTime);
};

#endif // FRMNTPCLIENT_H
