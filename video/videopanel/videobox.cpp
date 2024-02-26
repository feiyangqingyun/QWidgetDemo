#pragma execution_character_set("utf-8")

#include "videobox.h"
#include "qmenu.h"
#include "qaction.h"
#include "qgridlayout.h"
#include "qdebug.h"

VideoBox::VideoBox(QObject *parent) : QObject(parent)
{
    maxCount = 64;
    layoutType = "1_16";
    gridLayout = NULL;

    menuFlag = "画面";
    actionFlag = "通道";

    //通过这里设置好数据下面只需要循环添加和判断就行(灵活性大大增强/只需要这里改动下就行)

    //自定义x布局/按照行列数生成/可以通过appendtype函数添加其他类型
    //1_2x4表示通道1开始2x4行列布局画面(相当于通道1-8按照2行4列排列)
    //9_2x4表示通道9开始2x4行列布局画面(相当于通道9-16按照2行4列排列)
    types.insert("x", QStringList() << "1_1x1" << "1_4x1" << "1_2x4" << "9_2x4" << "1_3x2" << "1_4x2" << "1_5x2" << "1_6x2" << "1_7x2" << "1_8x2");

    //自定义y布局/主要是一些用户定义的不规则的排列布局/加个y用于区分其他布局/可能有雷同
    types.insert("y", QStringList() << "y_1_2" << "y_1_3" << "y_1_5" << "y_1_8" << "y_1_9" << "y_1_10" << "y_1_12" << "y_1_16");

    //1_4表示通道1-通道4/前面是通道开始的索引/后面是通道结束的索引
    types.insert("4", QStringList() << "1_4" << "5_8" << "9_12" << "13_16" << "17_20" << "21_24" << "25_28" << "29_32" << "33_36");
    types.insert("6", QStringList() << "1_6" << "7_12" << "13_18" << "19_24" << "25_30" << "31_36");
    types.insert("8", QStringList() << "1_8" << "9_16" << "17_24" << "25_32" << "33_40" << "41_48" << "49_57" << "57_64");
    types.insert("9", QStringList() << "1_9" << "9_17" << "18_26" << "27_35" << "36_42" << "43_50" << "51_59");
    types.insert("13", QStringList() << "1_13" << "14_26" << "27_39" << "40_52" << "52_64");
    types.insert("16", QStringList() << "1_16" << "17_32" << "33_48" << "49_64");
    types.insert("25", QStringList() << "1_25");
    types.insert("36", QStringList() << "1_36");
    types.insert("64", QStringList() << "1_64");

    //默认全部可见
    int count = types.count();
    for (int i = 0; i < count; ++i) {
        visibles << true;
    }
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
        } else if (type == "y") {
            start = list.at(1);
            end = list.at(2);
            start = QString("%1").arg(start, 2, QChar('0'));
            end = QString("%1").arg(end, 2, QChar('0'));
            text = QString("%1%2-%1%3").arg(actionFlag).arg(start).arg(end);
        } else {
            end = QString("%1").arg(end, 2, QChar('0'));
            text = QString("%1%2-%1%3").arg(actionFlag).arg(start).arg(end);
        }

        //只有一个节点则子菜单文字=主菜单文字
        if (flags.count() == 1) {
            text = name;
        }

        //添加菜单动作
        QAction *action = menuSub->addAction(text, this, SLOT(change_layout()));
        //设置弱属性传入大类和子类布局标识等
        action->setProperty("index", start);
        action->setProperty("type", type);
        action->setProperty("flag", flag);
    }
}

//行列数一致的比如 2*2 3*3 4*4 5*5 等可以直接套用通用的公式
//按照这个函数还可以非常容易的拓展出 10*10 16*16=256 通道界面
void VideoBox::change_layout_normal(int index, int row, int column)
{
    int size = 0;
    int rowCount = 0;
    int columnCount = 0;

    //首先隐藏所有通道
    hide_all();

    //按照指定的行列数逐个添加
    for (int i = 0; i < maxCount; ++i) {
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

void VideoBox::change_layout_custom(int index, int type)
{
    //从开始索引开始往后衍生多少个通道
    QList<int> indexs;
    for (int i = index; i < (index + type); ++i) {
        indexs << i;
    }

    //过滤防止索引越界
    if (indexs.count() < type) {
        return;
    }

    if (type == 6 || type == 8 || type == 10 || type == 12 || type == 16) {
        change_layout_l(indexs);
    } else if (type == 13) {
        change_layout_o(indexs);
    }
}

void VideoBox::change_layout_visible(int start, int end)
{
    //设置通道控件可见
    for (int i = start; i <= end; ++i) {
        widgets.at(i)->setVisible(true);
    }
}

void VideoBox::change_layout_l(const QList<int> &indexs)
{
    //通过观察发现这种都是左上角一个大通道/右侧和底部排列几个小通道
    int count = indexs.count();
    int num = count / 2;
    int flag = num - 1;

    //首先隐藏所有通道
    hide_all();

    //添加大通道
    gridLayout->addWidget(widgets.at(indexs.at(0)), 0, 0, flag, flag);
    //添加右侧小通道
    for (int i = 0; i < flag; ++i) {
        gridLayout->addWidget(widgets.at(indexs.at(i + 1)), i, flag);
    }
    //添加底部小通道
    for (int i = num; i < count; ++i) {
        gridLayout->addWidget(widgets.at(indexs.at(i)), flag, count - i - 1);
    }

    //下面添加6通道/这里留着挨个添加的写法/方便学习和对比
#if 0
    gridLayout->addWidget(widgets.at(indexs.at(0)), 0, 0, 2, 2);
    gridLayout->addWidget(widgets.at(indexs.at(1)), 0, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(2)), 1, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(3)), 2, 2, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(4)), 2, 1, 1, 1);
    gridLayout->addWidget(widgets.at(indexs.at(5)), 2, 0, 1, 1);
#endif

    //设置通道控件可见
    change_layout_visible(indexs.first(), indexs.last());
}

void VideoBox::change_layout_o(const QList<int> &indexs)
{
    //首先隐藏所有通道
    hide_all();
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
    change_layout_visible(indexs.first(), indexs.last());
}

QString VideoBox::getLayoutType() const
{
    return this->layoutType;
}

void VideoBox::setLayoutType(const QString &layoutType)
{
    this->layoutType = layoutType;
}

QWidgetList VideoBox::getWidgets() const
{
    return this->widgets;
}

void VideoBox::setWidgets(QWidgetList widgets)
{
    this->widgets = widgets;
    this->maxCount = widgets.count();
}

void VideoBox::setLayout(QGridLayout *gridLayout)
{
    this->gridLayout = gridLayout;
}

void VideoBox::setMenuFlag(const QString &menuFlag)
{
    this->menuFlag = menuFlag;
}

void VideoBox::setActionFlag(const QString &actionFlag)
{
    this->actionFlag = actionFlag;
}

void VideoBox::setVisibles(const QList<bool> &visibles)
{
    this->visibles = visibles;
}

void VideoBox::appendType(int index, int row, int column)
{
    //先要过滤下是否满足最大通道数量/start从1开始
    if (((index - 1) + (row * column)) > maxCount) {
        return;
    }

    //追加到x布局后面
    QString type = QString("%1_%2x%3").arg(index).arg(row).arg(column);
    QMap<QString, QStringList>::iterator iter = types.begin();
    while (iter != types.end()) {
        if (iter.key() == "x") {
            QStringList value = iter.value();
            if (!value.contains(type)) {
                value.append(type);
                types["x"] = value;
            }
            break;
        }
        iter++;
    }
}

void VideoBox::initMenu(QMenu *menu)
{
    //约定依次是按照顺序控制启用状态
    int count = visibles.count();
    if (count > 0 && visibles.at(0)) {
        addMenu(menu, "x");
    }
    if (count > 1 && visibles.at(1)) {
        addMenu(menu, "y");
    }
    if (count > 2 && visibles.at(2)) {
        addMenu(menu, "4");
    }
    if (count > 3 && visibles.at(3)) {
        addMenu(menu, "6");
    }
    if (count > 4 && visibles.at(4)) {
        addMenu(menu, "8");
    }
    if (count > 5 && visibles.at(5)) {
        addMenu(menu, "9");
    }
    if (count > 6 && visibles.at(6)) {
        addMenu(menu, "13");
    }
    if (count > 7 && visibles.at(7)) {
        addMenu(menu, "16");
    }
    if (count > 8 && visibles.at(8)) {
        addMenu(menu, "25");
    }
    if (count > 9 && visibles.at(9)) {
        addMenu(menu, "36");
    }
    if (count > 10 && visibles.at(10)) {
        addMenu(menu, "64");
    }
}

void VideoBox::show_all()
{
    //一般是从配置文件读取到了最后的通道画面类型进行设置
    int type = 1;
    int index = layoutType.split("_").first().toInt() - 1;
    //y开头的布局需要重置索引=0
    if (layoutType.startsWith("y")) {
        index = 0;
    }

    QMap<QString, QStringList>::iterator iter = types.begin();
    while (iter != types.end()) {
        QStringList flags = iter.value();
        if (flags.contains(layoutType)) {
            type = iter.key().toInt();
            change_layout(type, index);
            return;
        }
        iter++;
    }

    //如果运行到这里说明设置了不存在的布局/强制纠正
    layoutType = "1_4";
    this->show_all();
}

void VideoBox::hide_all()
{
    for (int i = 0; i < maxCount; ++i) {
        gridLayout->removeWidget(widgets.at(i));
        widgets.at(i)->setVisible(false);
    }
}

void VideoBox::change_layout()
{
    //识别具体是哪个动作菜单触发的
    QAction *action = (QAction *)sender();
    //从弱属性取出值
    int index = action->property("index").toInt() - 1;
    int type = action->property("type").toInt();
    QString layoutType = action->property("flag").toString();
    //只有当画面布局类型改变了才需要切换
    if (this->layoutType != layoutType) {
        this->layoutType = layoutType;
        change_layout(type, index);
    }
}

void VideoBox::change_layout(int type, int index)
{
    //根据不同的父菜单类型执行对应的函数
    if (type == 0) {
        if (layoutType.contains("x")) {
            //取出行列
            QString text = layoutType.split("_").last();
            QStringList list = text.split("x");
            int row = list.at(0).toInt();
            int column = list.at(1).toInt();
            change_layout_normal(index, row, column);

            //只有1个通道需要更改类型/方便外面区分当前是1通道
            if (layoutType.endsWith("1x1")) {
                type = 1;
            }
        } else if (layoutType == "y_1_2") {
            change_layout_y_1_2(index);
        } else if (layoutType == "y_1_3") {
            change_layout_y_1_3(index);
        } else if (layoutType == "y_1_5") {
            change_layout_y_1_5(index);
        } else if (layoutType == "y_1_8") {
            change_layout_y_1_8(index);
        } else if (layoutType == "y_1_9") {
            change_layout_y_1_9(index);
        } else if (layoutType == "y_1_10") {
            change_layout_y_1_10(index);
        } else if (layoutType == "y_1_12") {
            change_layout_y_1_12(index);
        } else if (layoutType == "y_1_16") {
            change_layout_y_1_16(index);
        }
    } else if (type == 1) {
        change_layout_1(index);
    } else if (type == 4) {
        change_layout_4(index);
    } else if (type == 6) {
        change_layout_6(index);
    } else if (type == 8) {
        change_layout_8(index);
    } else if (type == 9) {
        change_layout_9(index);
    } else if (type == 13) {
        change_layout_13(index);
    } else if (type == 16) {
        change_layout_16(index);
    } else if (type == 25) {
        change_layout_25(index);
    } else if (type == 36) {
        change_layout_36(index);
    } else if (type == 64) {
        change_layout_64(index);
    }

    Q_EMIT changeLayout(type, layoutType, false);
}

void VideoBox::change_layout_y_1_2(int index)
{
    change_layout_normal(index, 1, 2);
}

void VideoBox::change_layout_y_1_3(int index)
{
    //首先隐藏所有通道
    hide_all();
    //添加通道到布局
    gridLayout->addWidget(widgets.at(index + 0), 0, 0, 1, 2);
    gridLayout->addWidget(widgets.at(index + 1), 1, 0, 1, 1);
    gridLayout->addWidget(widgets.at(index + 2), 1, 1, 1, 1);
    //设置通道控件可见
    change_layout_visible(index, index + 2);
}

void VideoBox::change_layout_y_1_5(int index)
{
    //首先隐藏所有通道
    hide_all();
    //依次左上/左下/
    gridLayout->addWidget(widgets.at(index + 0), 0, 0, 1, 2);
    gridLayout->addWidget(widgets.at(index + 1), 1, 0, 2, 1);
    gridLayout->addWidget(widgets.at(index + 2), 1, 1, 1, 1);
    gridLayout->addWidget(widgets.at(index + 3), 2, 1, 1, 1);
    gridLayout->addWidget(widgets.at(index + 4), 0, 2, 3, 1);
    //设置通道控件可见
    change_layout_visible(index, index + 4);
}

void VideoBox::change_layout_y_1_8(int index)
{
    //首先隐藏所有通道
    hide_all();
    //添加上面4个通道
    gridLayout->addWidget(widgets.at(index + 0), 0, 0, 1, 1);
    gridLayout->addWidget(widgets.at(index + 1), 0, 1, 1, 1);
    gridLayout->addWidget(widgets.at(index + 2), 0, 2, 1, 1);
    gridLayout->addWidget(widgets.at(index + 3), 0, 3, 1, 1);
    //添加中间全景通道
    gridLayout->addWidget(widgets.at(index + 8), 1, 0, 1, 4);
    //添加下面4个通道
    gridLayout->addWidget(widgets.at(index + 4), 2, 0, 1, 1);
    gridLayout->addWidget(widgets.at(index + 5), 2, 1, 1, 1);
    gridLayout->addWidget(widgets.at(index + 6), 2, 2, 1, 1);
    gridLayout->addWidget(widgets.at(index + 7), 2, 3, 1, 1);
    //设置通道控件可见
    change_layout_visible(index, index + 8);
}

void VideoBox::change_layout_y_1_9(int index)
{
    //首先隐藏所有通道
    hide_all();
    //添加通道到布局
    gridLayout->addWidget(widgets.at(index + 0), 0, 0, 2, 2);
    gridLayout->addWidget(widgets.at(index + 1), 0, 2, 1, 1);
    gridLayout->addWidget(widgets.at(index + 2), 0, 3, 1, 1);
    gridLayout->addWidget(widgets.at(index + 3), 1, 2, 1, 1);
    gridLayout->addWidget(widgets.at(index + 4), 1, 3, 1, 1);
    gridLayout->addWidget(widgets.at(index + 5), 2, 3, 1, 1);
    gridLayout->addWidget(widgets.at(index + 6), 2, 2, 1, 1);
    gridLayout->addWidget(widgets.at(index + 7), 2, 1, 1, 1);
    gridLayout->addWidget(widgets.at(index + 8), 2, 0, 1, 1);
    //设置通道控件可见
    change_layout_visible(index, index + 8);
}

void VideoBox::change_layout_y_1_10(int index)
{
    change_layout_custom(index, 10);
}

void VideoBox::change_layout_y_1_12(int index)
{
    change_layout_custom(index, 12);
}

void VideoBox::change_layout_y_1_16(int index)
{
    change_layout_custom(index, 16);
}

void VideoBox::change_layout_1(int index)
{
    change_layout_normal(index, 1, 1);
}

void VideoBox::change_layout_4(int index)
{
    change_layout_normal(index, 2, 2);
}

void VideoBox::change_layout_6(int index)
{
    change_layout_custom(index, 6);
}

void VideoBox::change_layout_8(int index)
{
    change_layout_custom(index, 8);
}

void VideoBox::change_layout_9(int index)
{
    change_layout_normal(index, 3, 3);
}

void VideoBox::change_layout_13(int index)
{
    change_layout_custom(index, 13);
}

void VideoBox::change_layout_16(int index)
{
    change_layout_normal(index, 4, 4);
}

void VideoBox::change_layout_25(int index)
{
    change_layout_normal(index, 5, 5);
}

void VideoBox::change_layout_36(int index)
{
    change_layout_normal(index, 6, 6);
}

void VideoBox::change_layout_64(int index)
{
    change_layout_normal(index, 8, 8);
}
