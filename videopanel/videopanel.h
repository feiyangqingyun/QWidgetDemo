#ifndef VIDEOPANEL_H
#define VIDEOPANEL_H

/**
 * 视频监控画面控件 整理:feiyangqingyun(QQ:517216493) 2019-04-11
 * 1. 目前颜色都写死在样式表，可自行更改或者拓展属性设置。
 */

#include <QWidget>

class QMenu;
class QLabel;
class QGridLayout;

#ifdef quc
class Q_DECL_EXPORT VideoPanel : public QWidget
#else
class VideoPanel : public QWidget
#endif

{
    Q_OBJECT

public:
    explicit VideoPanel(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGridLayout *gridLayout;    //表格布局存放视频标签
    bool videoMax;              //是否最大化
    int videoCount;             //视频通道个数
    QString videoType;          //当前画面类型
    QMenu *videoMenu;           //右键菜单
    QAction *actionFull;        //全屏动作
    QAction *actionPoll;        //轮询动作
    QList<QLabel *> widgets;    //视频标签集合

public:
    QSize sizeHint()            const;
    QSize minimumSizeHint()     const;

private slots:
    void initControl();
    void initForm();
    void initMenu();
    void full();
    void poll();

private slots:
    void play_video_all();
    void snapshot_video_one();
    void snapshot_video_all();

    void show_video_all();
    void show_video_4();
    void show_video_6();
    void show_video_8();
    void show_video_9();
    void show_video_13();
    void show_video_16();
    void show_video_25();
    void show_video_36();
    void show_video_64();

    void hide_video_all();
    void change_video(int index, int flag);
    void change_video_4(int index);
    void change_video_6(const QList<int> &indexs);
    void change_video_6(int index);
    void change_video_8(const QList<int> &indexs);
    void change_video_8(int index);
    void change_video_9(int index);
    void change_video_13(const QList<int> &indexs);
    void change_video_13(int index);
    void change_video_16(int index);
    void change_video_25(int index);
    void change_video_36(int index);
    void change_video_64(int index);

signals:
    void fullScreen(bool full);
};

#endif // VIDEOPANEL_H
