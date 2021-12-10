#include "frmmultiaxes.h"
#include "ui_frmmultiaxes.h"
#include "qfile.h"
#include "qdebug.h"

frmMultiAxes::frmMultiAxes(QWidget *parent) : QWidget(parent), ui(new Ui::frmMultiAxes)
{
    ui->setupUi(this);
    this->initForm();
}

frmMultiAxes::~frmMultiAxes()
{
    delete ui;
}

void frmMultiAxes::initForm()
{
    //Y坐标轴的数量
    count = 4;

    this->initPlot(ui->customPlot1, QCPAxis::atLeft, count);
    this->initPlot(ui->customPlot2, QCPAxis::atRight, count);

    this->loadPlot(ui->customPlot1, QCPAxis::atLeft, count);
    this->loadPlot(ui->customPlot2, QCPAxis::atRight, count);

    for (int i = 0; i < count; ++i) {
        this->loadData(ui->customPlot1, i);
        this->loadData(ui->customPlot2, i);
    }
}

void frmMultiAxes::selectionChangedByUser()
{
    //联动处理 坐标轴、刻度尺、画布曲线 选中一样则选中所有
    QCustomPlot *customPlot = (QCustomPlot *)sender();
    QCPAxis::AxisType type;
    if (customPlot == ui->customPlot1) {
        type = QCPAxis::atLeft;
    } else if (customPlot == ui->customPlot2) {
        type = QCPAxis::atRight;
    }

    int selecteIndex = -1;
    for (int i = 0; i < count; ++i) {
        //获取对应的坐标轴
        QCPAxis *axis = customPlot->axisRect()->axis(type, i);
        //获取对应的画布曲线
        QCPGraph *graph = customPlot->graph(i);
        //判断 坐标轴、刻度尺、画布曲线 是否选中了一个
        if (axis->selectedParts().testFlag(QCPAxis::spAxis) || axis->selectedParts().testFlag(QCPAxis::spTickLabels) || graph->selected()) {
            //设置坐标轴、刻度尺同时选中
            axis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
            //设置画布曲线选中
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            selecteIndex = i;
            break;
        }
    }

    //存在选中则设置当前选中的可拖动和缩放
    //不存在则设置所有的可拖动和缩放
    QCPAxisRect *axisRect = customPlot->axisRect();
    if (selecteIndex >= 0) {
        QCPAxis *axis = axisRect->axis(type, selecteIndex);
        axisRect->setRangeDragAxes(customPlot->xAxis, axis);
        axisRect->setRangeZoomAxes(customPlot->xAxis, axis);
    } else {
#ifndef qcustomplot_v1_3
        axisRect->setRangeDragAxes(axisRect->axes());
        axisRect->setRangeZoomAxes(axisRect->axes());
#endif
    }
}

void frmMultiAxes::initPlot(QCustomPlot *customPlot, const QCPAxis::AxisType &type, int count)
{
    //选中对应的坐标轴或者画布等
    connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChangedByUser()));
    //connect(customPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis, SLOT(setRange(QCPRange)));

    if (type == QCPAxis::atLeft) {
        customPlot->yAxis->setVisible(true);
        customPlot->yAxis2->setVisible(false);
    } else if (type == QCPAxis::atRight) {
        customPlot->yAxis->setVisible(false);
        customPlot->yAxis2->setVisible(true);
    }

    //添加多个坐标轴
    for (int i = 0; i < count - 1; ++i) {
        customPlot->axisRect()->addAxis(type);
    }

    //设置边距、范围值、网格可见、刻度数量等
    for (int i = 0; i < count; ++i) {
        QCPAxis *axis = customPlot->axisRect()->axis(type, i);
        axis->setPadding(10);
        axis->setRange(0, 1);
        axis->grid()->setVisible(true);
        //axis->grid()->setSubGridVisible(true);
        axis->ticker()->setTickCount(10);
    }

    //设置初始范围值
    customPlot->xAxis->setRange(0.5, 10.5);
    //设置刻度数量
    customPlot->xAxis->ticker()->setTickCount(10);
    //设置可拖动和缩放选中等
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectPlottables);
}

void frmMultiAxes::loadPlot(QCustomPlot *customPlot, const QCPAxis::AxisType &type, int count)
{
    //颜色集合
    QList<QColor> colors;
    colors << QColor(211, 78, 78) << QColor(29, 185, 242) << QColor(170, 162, 119) << QColor(255, 192, 1);
    colors << QColor(0, 176, 180) << QColor(0, 113, 193) << QColor(255, 192, 0);
    colors << QColor(72, 103, 149) << QColor(185, 87, 86) << QColor(0, 177, 125);
    colors << QColor(214, 77, 84) << QColor(71, 164, 233) << QColor(34, 163, 169);
    colors << QColor(59, 123, 156) << QColor(162, 121, 197) << QColor(72, 202, 245);
    colors << QColor(0, 150, 121) << QColor(111, 9, 176) << QColor(250, 170, 20);

    for (int i = 0; i < count; ++i) {
        //分配一种颜色
        QColor color;
        if (i < colors.count()) {
            color = colors.at(i);
        }

        QPen pen(color);
        QCPAxis *axis = customPlot->axisRect()->axis(type, i);
        //刻度文本颜色
        axis->setTickLabelColor(color);
        //刻度线条颜色
        axis->setBasePen(pen);
        //大刻度尺颜色
        axis->setTickPen(pen);
        //小刻度尺颜色
        axis->setSubTickPen(pen);
        //画布线条颜色
        QCPGraph *graph = customPlot->addGraph(customPlot->xAxis, axis);
        graph->setPen(pen);
    }
}

void frmMultiAxes::loadData(QCustomPlot *customPlot, int index)
{
    //下面演示从文本文件读取数据，具体格式可以自行打开文件查看
    //真实场景自己准备数据调用 setData 即可
    int flag = index > 2 ? 0 : index;
    QString fileName = QString(":/data/data%1.txt").arg(flag);
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QIODevice::Text)) {
        QString value = file.readAll();
        file.close();

        QVector<double> keys, values;
        QStringList list = value.split(" ");
        int len = list.length();

        //如果不是2的倍数则长度要减去1
        if (len % 2 != 0) {
            len = (len - 1);
        }

        for (int i = 0; i < len / 2; i++) {
            keys << i;
        }

        //每两个16进制数字组合成一个数字
        for (int i = 0; i < len; i = i + 2) {
            QString strHex = QString("%1%2").arg(list.at(i)).arg(list.at(i + 1));
            qint16 value = strHex.toUShort(NULL, 16);
            values << value;
        }

        //获取对应画布
        QCPGraph *graph = customPlot->graph(index);
        //设置数据到画布
        graph->setData(keys, values);
        //自适应坐标轴
        graph->rescaleAxes();
    }
}
