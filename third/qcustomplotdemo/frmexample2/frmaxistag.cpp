#include "frmaxistag.h"
#include "ui_frmaxistag.h"
#include "qdebug.h"

frmAxisTag::frmAxisTag(QWidget *parent) : QWidget(parent), ui(new Ui::frmAxisTag)
{
    ui->setupUi(this);
    this->initForm();
}

frmAxisTag::~frmAxisTag()
{
    delete ui;
}

void frmAxisTag::initForm()
{
    mPlot = ui->customPlot;

    // configure plot to have two right axes:
    mPlot->yAxis->setTickLabels(false);
    connect(mPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), mPlot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
    mPlot->yAxis2->setVisible(true);
    mPlot->axisRect()->addAxis(QCPAxis::atRight);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(30); // add some padding to have space for tags
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(30); // add some padding to have space for tags

    // create graphs:
    mGraph1 = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 0));
    mGraph2 = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 1));
    mGraph1->setPen(QPen(QColor(250, 120, 0)));
    mGraph2->setPen(QPen(QColor(0, 180, 60)));

    // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
    mTag1 = new AxisTag(mGraph1->valueAxis());
    mTag1->setPen(mGraph1->pen());
    mTag2 = new AxisTag(mGraph2->valueAxis());
    mTag2->setPen(mGraph2->pen());

    connect(&mDataTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    mDataTimer.start(40);
}

void frmAxisTag::timerSlot()
{
    // calculate and add a new data point to each graph:
    int count1 = mGraph1->dataCount();
    mGraph1->addData(count1, qSin(count1 / 50.0) + qSin(count1 / 50.0 / 0.3843) * 0.25);
    int count2 = mGraph2->dataCount();
    mGraph2->addData(count2, qCos(count2 / 50.0) + qSin(count2 / 50.0 / 0.4364) * 0.15);

    // make key axis range scroll with the data:
    mPlot->xAxis->rescale();
    mGraph1->rescaleValueAxis(false, true);
    mGraph2->rescaleValueAxis(false, true);
    mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 100, Qt::AlignRight);

    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double graph1Value = mGraph1->dataMainValue(mGraph1->dataCount() - 1);
    double graph2Value = mGraph2->dataMainValue(mGraph2->dataCount() - 1);
    mTag1->updatePosition(graph1Value);
    mTag2->updatePosition(graph2Value);
    mTag1->setText(QString::number(graph1Value, 'f', 2));
    mTag2->setText(QString::number(graph2Value, 'f', 2));

    mPlot->replot();
}
