#pragma execution_character_set("utf-8")

#include "videobox.h"
#include "qmenu.h"
#include "qaction.h"
#include "qgridlayout.h"
#include "qdebug.h"

VideoBox::VideoBox(QObject *parent) : QObject(parent)
{
    gridLayout = 0;
    videoCount = 64;
    videoType = "1_16";

    enableOther = false;
    menuFlag = "画面";
    actionFlag = "通道";

    //通过这里设置好数据下面只需要循环添加和判断就行(灵活性大大增强/只需要这里改动下就行)
    //1_2表示通道1-通道2/前面是通道开始的索引/后面是通道结束的索引
    //1_2x4表示通道1开始2x4行列布局画面(相当于通道1-8按照2行4列排列)
    //9_2x4表示通道9开始2x4行列布局画面(相当于通道9-16按照2行4列排列)
    types.insert("2", QStringList() << "1_2");
    types.insert("3", QStringList() << "1_3");
    //按照行列数生成的可以自行添加其他布局
    types.insert("x", QStringList() << "1_4x1" << "1_2x4" << "9_2x4" << "1_4x2" << "9_4x2" << "1_2x5" << "10_2x5" << "1_5x2" << "1_6x2" << "1_7x2");
    types.insert("4", QStringList() << "1_4" << "5_8" << "9_12" << "13_16" << "17_20" << "21_24" << "25_28" << "29_32" << "33_36");
    types.insert("6", QStringList() << "1_6" << "7_12" << "13_18" << "19_24" << "25_30" << "31_36");
    types.insert("8", QStringList() << "1_8" << "9_16" << "17_24" << "25_32" << "33_40" << "41_48" << "49_57" << "57_64");
    types.insert("9", QStringList() << "1_9" << "9_17" << "18_26" << "27_35" << "36_42" << "43_50" << "51_59");
    types.insert("13", QStringList() << "1_13" << "14_26" << "27_39" << "40_52" << "52_64");
    types.insert("16", QStringList() << "1_16" << "17_32" << "33_48" << "49_64");
    types.insert("25", QStringList() << "1_25");
    types.insert("36", QStringList() << "1_36");
    types.insert("64", QStringList() << "1_64");
}

void VideoBox::addMenu(QMenu *menu, const QString &type)
{
    //父菜单文字
    QString name = QString("切换到%1%2").arg(type).arg(menuFlag);
    //链表中取出该布局大类下的小类集合
    QStringList flags = types.value(type);

    //超过一个子元素则添加子菜单
    QMenu *menuSub;
    if (flags.count() > 1) {
        menuSub = menu->addMenu(name);
    } else {
        menuSub = menu;
    }

    foreach (QString flag, flags) {
        QString text;
        QStringList list = flag.split("_");
        QString start = list.at(0);
        QString end = list.at(1);
        start = QString("%1").arg(start, 2, QChar('0'));

        //对应菜单文本
        if (type == "x") {
            list = end.split("x");
            text = QString("%1 x %2").arg(list.at(0)).arg(list.at(1));
            text = QString("%1%2 (%3)").arg(actionFlag).arg(start).arg(text);
        } else {
            end = QString("%1").arg(end, 2, QChar('0'));
            text = QString("%1%2-%1%3").arg(actionFlag).arg(start).arg(end);
        }

        //只有一个节点则子菜单文字=主菜单文字
        if (flags.count() == 1) {
            text = name;
        }

        //添加菜单动作
        QAction *action = menuSub->addAction(text, this, SLOT(show_video()));
        //设置弱属性传入大类和子类布局标识等
        action->setProperty("index", start);
        action->setProperty("type", type);
        action->setProperty("flag", flag);
    }
}

QString VideoBox::getVideoType() const
{
    return this->videoType;
}

void VideoBox::setVideoType(const QString &videoType)
{
    this->videoType = videoType;
}

QWidgetList VideoBox::getWidgets() const
{
    return this->widgets;
}

void VideoBox::setWidgets(QWidgetList widgets)
{
    this->widgets = widgets;
    this->videoCount = widgets.count();
}

void VideoBox::setLayout(QGridLayout *gridLayout)
{
    this->gridLayout = gridLayout;
}

void VideoBox::setEnableOther(bool enableOther)
{
    this->enableOther = enableOther;
}

void VideoBox::setMenuFlag(const QString &menuFlag)
{
    this->menuFlag = menuFlag;
}

void VideoBox::setActionFlag(const QString &actionFlag)
{
    this->actionFlag = actionFlag;
}

void VideoBox::setTypes(const QMap<QString, QStringList> &types)
{
    this->types = types;
}

void VideoBox::initMenu(QMenu *menu, const QList<bool> &enable)
{
    if (enableOther) {
        addMenu(menu, "2");
        addMenu(menu, "3");
        addMenu(menu, "x");
    }

    //通过菜单是否可见设置每个菜单可见与否
    if (enable.count() < 9) {
        return;
    }

    if (enable.at(0)) {
        addMenu(menu, "4");
    }
    if (enable.at(1)) {
        addMenu(menu, "6");
    }
    if (enable.at(2)) {
        addMenu(menu, "8");
    }
    if (enable.at(3)) {
        addMenu(menu, "9");
    }
    if (enable.at(4)) {
        addMenu(menu, "13");
    }
    if (enable.at(5)) {
        addMenu(menu, "16");
    }
    if (enable.at(6)) {
        addMenu(menu, "25");
    }
    if (enable.at(7)) {
        addMenu(menu, "36");
    }
    if (enable.at(8)) {
        addMenu(menu, "64");
    }
}

void VideoBox::show_video(int type, int index)
{
    //根据不同的父菜单类型执行对应的函数
    if (type == 0) {
        //取出行列
        QString text = videoType.split("_").last();
        QStringList list = text.split("x");
        int row = list.at(0).toInt();
        int column = list.at(1).toInt();
        change_video_normal(index, row, column);
    } else if (type == 1) {
        change_video_1(index);
    } else if (type == 2) {
        change_video_2(index);
    } else if (type == 3) {
        change_video_3(index);
    } else if (type == 4) {
        change_video_4(index);
    } else if (type == 6) {
        change_video_6(index);
    } else if (type == 8) {
        change_video_8(index);
    } else if (type == 9) {
        change_video_9(index);
    } else if (type == 13) {
        change_video_13(index);
    } else if (type == 16) {
        change_video_16(index);
    } else if (type == 25) {
        change_video_25(index);
    } else if (type == 36) {
        change_video_36(index);
    } else if (type == 64) {
        change_video_64(index);
    }

    Q_EMIT changeVideo(type, videoType, false);
}

void VideoBox::show_video()
{
    //识别具体是哪个动作菜单触发的
    QAction *action = (QAction *)sender();
    //从弱属性取出值
    int index = action->property("index").toInt() - 1;
    int type = action->property("type").toInt();
    QString videoType = action->property("flag").toString();

    //只有当画面布局类型改变了才需要切换
    if (this->videoType != videoType) {
        this->videoType = videoType;
        show_video(type, index);
    }
}

void VideoBox::show_video_all()
{
    //一般是从配置文件读取到了最后的通道画面类型进行设置
    int type = 1;
    if (videoType.startsWith("0_")) {
        int index = videoType.split("_").last().toInt() - 1;
        change_video_1(index);
        Q_EMIT changeVideo(type, videoType, true);
    } else {
        int index = videoType.split("_").first().toInt() - 1;
        QMap<QString, QStringList>::iterator iter = types.begin();
        while (iter != types.end()) {
            QStringList flags = iter.value();
            if (flags.contains(videoType)) {
                type = iter.key().toInt();
                show_video(type, index);
                break;
            }
            iter++;
        }
    }
}

void VideoBox::hide_video_all()
{
    for (int i = 0; i < videoCount; ++i) {
        gridLayout->removeWidget(widgets.at(i));
        widgets.at(i)->setVisible(false);
    }
}

//行列数一致的比如 2*2 3*3 4*4 5*5 等可以直接套用通用的公式
//按照这个函数还可以非常容易的拓展出 10*10 16*16=256 通道界面
void VideoBox::change_video_normal(int index, int row, int column)
{
    int size = 0;
    int rowCount = 0;
    int columnCount = 0;

    //首先隐藏所有通道
    hide_video_all();

    //按照指定的行列数逐个添加
    for (int i = 0; i < videoCount; ++i) {
        if (i >= index) {
            //添加到对应布局并设置可见
            gridLayout->addWidget(widgets.at(i), rowCount, columnCount);
            widgets.at(i)->setVisible(true);

            size++;
            columnCount++;
            if (columnCount == column) {
                rowCount++;
                columnCount = 0;
            }
        }

        //到了规定的数量则不用继续
        if (size == (row * column)) {
            break;
        }
    }
}

void VideoBox::change_video_custom(int index, int type)
{
    //从开始索引开始往后衍生多少个通道
    QList<int> indexs;
    for (int i = index; i < (index + type); ++i) {
        indexs << i;
    }

    if (type == 6) {
        change_video_6(indexs);
    } else if (type == 8) {
        change_video_8(indexs);
    } else if (type == 13) {
        change_video_13(indexs);
    }
}

void VideoBox::change_video_visible(int start, int end)
{
    //设置通道控件可见
    for (int i = start; i <= end; ++i) {
        widgets.at(i)->setVisible(true);
    }
}

void VideoBox::change_video_6(const QList<int> &indexs)
{
    //过滤防止索引越界
    if (indexs.count() < 6) {
        return;
    }

    //首先隐藏所有通道
    hide_video_all();
    //挨个重新添加到布局
    gridLayout->addWidget(widgets.at(indexs.at(0)), 0, 0, 2, 2);
    gridLayout->addWidget(widgets.at(indexs.at(1)), 0, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(2)), 1, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(3)), 2, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(4)), 2, 1, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(5)), 2, 0, 1, 1);
    //设置通道控件可见
    change_video_visible(indexs.first(), indexs.last());
}

void VideoBox::change_video_8(const QList<int> &indexs)
{
    //过滤防止索引越界
    if (indexs.count() < 8) {
        return;
    }

    //首先隐藏所有通道
    hide_video_all();
    //挨个重新添加到布局
    gridLayout->addWidget(widgets.at(indexs.at(0)), 0, 0, 3, 3);
    gridLayout->addWidget(widgets.at(indexs.at(1)), 0, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(2)), 1, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(3)), 2, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(4)), 3, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(5)), 3, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(6)), 3, 1, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(7)), 3, 0, 1, 1);
    //设置通道控件可见
    change_video_visible(indexs.first(), indexs.last());
}

void VideoBox::change_video_13(const QList<int> &indexs)
{
    //过滤防止索引越界
    if (indexs.count() < 13) {
        return;
    }

    //首先隐藏所有通道
    hide_video_all();
    //挨个重新添加到布局
    gridLayout->addWidget(widgets.at(indexs.at(0)), 0, 0, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(1)), 0, 1, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(2)), 0, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(3)), 0, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(4)), 1, 0, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(5)), 2, 0, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(6)), 1, 1, 2, 2);
    gridLayout->addWidget(widgets.at(indexs.at(7)), 1, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(8)), 2, 3, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(9)), 3, 0, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(10)), 3, 1, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(11)), 3, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(12)), 3, 3, 1, 1);
    //设置通道控件可见
    change_video_visible(indexs.first(), indexs.last());
}

void VideoBox::change_video_1(int index)
{
    //首先隐藏所有通道
    hide_video_all();
    //添加通道到布局
    gridLayout->addWidget(widgets.at(index), 0, 0);
    //设置通道控件可见
    widgets.at(index)->setVisible(true);
}

void VideoBox::change_video_2(int index)
{
    //首先隐藏所有通道
    hide_video_all();
    //添加通道到布局
    gridLayout->addWidget(widgets.at(index), 0, 0);
    gridLayout->addWidget(widgets.at(index + 1), 0, 1);
    //设置通道控件可见
    change_video_visible(index, index + 1);
}

void VideoBox::change_video_3(int index)
{
    //首先隐藏所有通道
    hide_video_all();
    //添加通道到布局
    gridLayout->addWidget(widgets.at(index), 0, 0, 1, 2);
    gridLayout->addWidget(widgets.at(index + 1), 1, 0);
    gridLayout->addWidget(widgets.at(index + 2), 1, 1);
    //设置通道控件可见
    change_video_visible(index, index + 2);
}

void VideoBox::change_video_4(int index)
{
    change_video_normal(index, 2, 2);
}

void VideoBox::change_video_6(int index)
{
    change_video_custom(index, 6);
}

void VideoBox::change_video_8(int index)
{
    change_video_custom(index, 8);
}

void VideoBox::change_video_9(int index)
{
    change_video_normal(index, 3, 3);
}

void VideoBox::change_video_13(int index)
{
    change_video_custom(index, 13);
}

void VideoBox::change_video_16(int index)
{
    change_video_normal(index, 4, 4);
}

void VideoBox::change_video_25(int index)
{
    change_video_normal(index, 5, 5);
}

void VideoBox::change_video_36(int index)
{
    change_video_normal(index, 6, 6);
}

void VideoBox::change_video_64(int index)
{
    change_video_normal(index, 8, 8);
}
