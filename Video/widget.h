#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Windows.h>
#include "HCNetSDK.h"
#include <QMouseEvent>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_close_clicked();
    void closeEvent(QCloseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *);

private:
    Ui::Widget *ui;
    QString host;
    int port;
    QString username;
    QString password;

    long userID;
    long previewID;

    bool getCameraInfo(void);
    bool initSDK();
    bool uninitSDK();
    long play(HWND hWnd, NET_DVR_PREVIEWINFO struPlayInfo);
    void OnClickedLogin();
    void OnClickedLogout();
    void OnClickedPreview();
    void OnClickedStopPreview();
    void OnClickedCapture();

    char *QString_2_char(QString &uu);
};

#endif // WIDGET_H
