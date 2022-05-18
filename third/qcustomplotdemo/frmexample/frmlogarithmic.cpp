#include "frmlogarithmic.h"
#include "ui_frmlogarithmic.h"
#include "qdebug.h"

frmLogarithmic::frmLogarithmic(QWidget *parent) : QWidget(parent), ui(new Ui::frmLogarithmic)
{
    ui->setupUi(this);
    this->initForm();
}

frmLogarithmic::~frmLogarithmic()
{
    delete ui;
}

void frmLogarithmic::initForm()
{
    ui->customPlot->setNoAntialiasingOnDrag(true); // more performance/responsiveness during dragging
    ui->customPlot->addGraph();

    QPen pen;
    pen.setColor(QColor(255, 170, 100));
    pen.setWidth(2);
    pen.setStyle(Qt::DotLine);
    ui->customPlot->graph(0)->setPen(pen);
    ui->customPlot->graph(0)->setName("x");

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setBrush(QBrush(QColor(255, 0, 0, 20)));
    ui->customPlot->graph(1)->setName("-sin(x)exp(x)");

    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(2)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    ui->customPlot->graph(2)->setName(" sin(x)exp(x)");

    ui->customPlot->addGraph();
    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    ui->customPlot->graph(3)->setPen(pen);
    ui->customPlot->graph(3)->setBrush(QBrush(QColor(0, 0, 0, 15)));
    ui->customPlot->graph(3)->setLineStyle(QCPGraph::lsStepCenter);
    ui->customPlot->graph(3)->setName("x!");

    const int dataCount = 200;
    const int dataFactorialCount = 21;
    QVector<QCPGraphData> dataLinear(dataCount), dataMinusSinExp(dataCount), dataPlusSinExp(dataCount), dataFactorial(dataFactorialCount);
    for (int i = 0; i < dataCount; ++i) {
        dataLinear[i].key = i / 10.0;
        dataLinear[i].value = dataLinear[i].key;
        dataMinusSinExp[i].key = i / 10.0;
        dataMinusSinExp[i].value = -qSin(dataMinusSinExp[i].key) * qExp(dataMinusSinExp[i].key);
        dataPlusSinExp[i].key = i / 10.0;
        dataPlusSinExp[i].value = qSin(dataPlusSinExp[i].key) * qExp(dataPlusSinExp[i].key);
    }

    for (int i = 0; i < dataFactorialCount; ++i) {
        dataFactorial[i].key = i;
        dataFactorial[i].value = 1.0;
        for (int k = 1; k <= i; ++k) {
            dataFactorial[i].value *= k;    // factorial
        }
    }

    ui->customPlot->graph(0)->data()->set(dataLinear);
    ui->customPlot->graph(1)->data()->set(dataMinusSinExp);
    ui->customPlot->graph(2)->data()->set(dataPlusSinExp);
    ui->customPlot->graph(3)->data()->set(dataFactorial);

    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->xAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->customPlot->yAxis->setTicker(logTicker);
    ui->customPlot->yAxis2->setTicker(logTicker);
    ui->customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    ui->customPlot->xAxis->setRange(0, 19.9);
    ui->customPlot->yAxis->setRange(1e-2, 1e10);    

    // make top right axes clones of bottom left axes:
    ui->customPlot->axisRect()->setupFullAxesBox();
    // connect signals so top and right axes move in sync with bottom and left axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 150)));
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop); // make legend align in top left corner or axis rect
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
