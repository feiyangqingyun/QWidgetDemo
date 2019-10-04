#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

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
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmMain *ui;

    bool videoMax;
    int videoCount;
    QString videoType;
    QMenu *videoMenu;
    QList<QLabel *> widgets;

private slots:
    void initForm();
    void initMenu();

private slots:
    void play_video_all();
    void snapshot_video_one();
    void snapshot_video_all();

    void show_video_all();
    void show_video_4();
    void show_video_6();
    void show_video_8();
    void show_video_9();
    void show_video_16();

    void hide_video_all();
    void change_video(int index, int flag);
    void change_video_4(int index);
    void change_video_6(int index);
    void change_video_8(int index);
    void change_video_9(int index);
    void change_video_16(int index);
};

#endif // FRMMAIN_H
