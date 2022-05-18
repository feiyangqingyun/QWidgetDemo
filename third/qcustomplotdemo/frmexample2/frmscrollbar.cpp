#include "frmscrollbar.h"
#include "ui_frmscrollbar.h"
#include "qdebug.h"

frmScrollBar::frmScrollBar(QWidget *parent) : QWidget(parent), ui(new Ui::frmScrollBar)
{
    ui->setupUi(this);
    this->initForm();
}

frmScrollBar::~frmScrollBar()
{
    delete ui;
}

void frmScrollBar::initForm()
{
    // The following plot setup is mostly taken from the plot demos:
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setPen(QPen(Qt::blue));
    ui->customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setPen(QPen(Qt::red));

    QVector<double> x(500), y0(500), y1(500);
    for (int i = 0; i < 500; ++i) {
        x[i] = (i / 499.0 - 0.5) * 10;
        y0[i] = qExp(-x[i] * x[i] * 0.25) * qSin(x[i] * 5) * 5;
        y1[i] = qExp(-x[i] * x[i] * 0.25) * 5;
    }

    ui->customPlot->graph(0)->setData(x, y0);
    ui->customPlot->graph(1)->setData(x, y1);
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // configure scroll bars:
    // Since scroll bars only support integer values, we'll set a high default range of -500..500 and
    // divide scroll bar position values by 100 to provide a scroll range -5..5 in floating point
    // axis coordinates. if you want to dynamically grow the range accessible with the scroll bar,
    // just increase the minimum/maximum values of the scroll bars as needed.
    ui->horizontalScrollBar->setRange(-500, 500);
    ui->verticalScrollBar->setRange(-500, 500);

    // create connection between axes and scroll bars:
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));

    // initialize axis range (and scroll bar positions via signals we just connected):
    ui->customPlot->xAxis->setRange(0, 6, Qt::AlignCenter);
    ui->customPlot->yAxis->setRange(0, 10, Qt::AlignCenter);
}

void frmScrollBar::horzScrollBarChanged(int value)
{
    if (qAbs(ui->customPlot->xAxis->range().center() - value / 100.0) > 0.01) { // if user is dragging plot, we don't want to replot twice
        ui->customPlot->xAxis->setRange(value / 100.0, ui->customPlot->xAxis->range().size(), Qt::AlignCenter);
        ui->customPlot->replot();
    }
}

void frmScrollBar::vertScrollBarChanged(int value)
{
    if (qAbs(ui->customPlot->yAxis->range().center() + value / 100.0) > 0.01) { // if user is dragging plot, we don't want to replot twice
        ui->customPlot->yAxis->setRange(-value / 100.0, ui->customPlot->yAxis->range().size(), Qt::AlignCenter);
        ui->customPlot->replot();
    }
}

void frmScrollBar::xAxisChanged(QCPRange range)
{
    ui->horizontalScrollBar->setValue(qRound(range.center() * 100.0)); // adjust position of scroll bar slider
    ui->horizontalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}

void frmScrollBar::yAxisChanged(QCPRange range)
{
    ui->verticalScrollBar->setValue(qRound(-range.center() * 100.0)); // adjust position of scroll bar slider
    ui->verticalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}
