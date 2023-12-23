#ifndef VIDEOBOX_H
#define VIDEOBOX_H

/**
 * 监控画面切换控件 作者:feiyangqingyun(QQ:517216493) 2021-11-08
 * 1. 将所有通道切换处理全部集中到一个类。
 * 2. 通用整数倍数布局切换函数，比如4x4/5x5，可方便拓展到100/255通道等。
 * 3. 通用行列数量布局切换函数，比如1x4/4x2，可方便拓展各种axb行列数。
 * 4. 通用特殊异形布局切换函数，可以参考进行自定义异形布局。
 * 5. 提供添加行列数布局函数。
 * 6. 通道布局切换发出信号通知。
 * 7. 可控每种布局切换菜单是否启用。
 * 8. 支持自定义子菜单布局内容。
 * 9. 支持设置对应的菜单标识比如默认的通道字样改成设备。
 */

#include <QObject>
#include <QWidget>
#include <QMap>

class QMenu;
class QWidget;
class QGridLayout;

#ifdef quc
class Q_DECL_EXPORT VideoBox : public QObject
#else
class VideoBox : public QObject
#endif

{
    Q_OBJECT

public:
    explicit VideoBox(QObject *parent = 0);

private:
    //表格布局存放通道
    QGridLayout *gridLayout;
    //视频控件集合
    QWidgetList widgets;

    //通道数量
    int videoCount;
    //当前画面类型
    QString videoType;

    //主菜单子菜单文字标识
    QString menuFlag;
    QString actionFlag;

    //可以控制每个子菜单是否可见
    QList<bool> visibles;

    //布局方案标识集合
    QMap<QString, QStringList> types;
    void addMenu(QMenu *menu, const QString &type);

private:
    //常规及异形通道布局
    void change_video_normal(int index, int row, int column);
    void change_video_custom(int index, int type);

    //设置可见
    void change_video_visible(int start, int end);

    //异形布局(l表示右侧底部环绕布局/o表示上下左右环绕布局)
    void change_video_l(const QList<int> &indexs);
    void change_video_o(const QList<int> &indexs);

public:
    //获取和设置当前画面类型
    QString getVideoType() const;
    void setVideoType(const QString &videoType);

    //获取和设置视频控件集合
    QWidgetList getWidgets() const;
    void setWidgets(QWidgetList widgets);

    //设置表格布局
    void setLayout(QGridLayout *gridLayout);

    //设置主菜单子菜单文字标识
    void setMenuFlag(const QString &menuFlag);
    void setActionFlag(const QString &actionFlag);

    //设置子菜单可见
    void setVisibles(const QList<bool> &visibles);

    //添加行列布局(必须在initMenu前调用)
    void appendType(int index, int row, int column);
    //初始化菜单
    void initMenu(QMenu *menu);

public Q_SLOTS:
    //显示和隐藏所有通道
    void show_video_all();
    void hide_video_all();

    //菜单切换布局槽函数
    void show_video();
    void show_video(int type, int index);

    //自定义布局通道切换函数
    void change_video_y_1_2(int index);
    void change_video_y_1_3(int index);
    void change_video_y_1_9(int index);
    void change_video_y_1_10(int index);
    void change_video_y_1_12(int index);
    void change_video_y_1_16(int index);

    //常规布局通道切换函数
    void change_video_1(int index);
    void change_video_4(int index);
    void change_video_6(int index);
    void change_video_8(int index);
    void change_video_9(int index);
    void change_video_13(int index);
    void change_video_16(int index);
    void change_video_25(int index);
    void change_video_36(int index);
    void change_video_64(int index);

Q_SIGNALS:
    //画面布局切换信号
    void changeVideo(int type, const QString &videoType, bool videoMax);
};

#endif // VIDEOBOX_H
