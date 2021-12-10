#include "frmtexturebrush.h"
#include "ui_frmtexturebrush.h"
#include "qdebug.h"

frmTextureBrush::frmTextureBrush(QWidget *parent) : QWidget(parent), ui(new Ui::frmTextureBrush)
{
    ui->setupUi(this);
    this->initForm();
}

frmTextureBrush::~frmTextureBrush()
{
    delete ui;
}

void frmTextureBrush::initForm()
{
    // add two graphs with a textured fill:
    ui->customPlot->addGraph();
    QPen redDotPen;
    redDotPen.setStyle(Qt::DotLine);
    redDotPen.setColor(QColor(170, 100, 100, 180));
    redDotPen.setWidthF(2);
    ui->customPlot->graph(0)->setPen(redDotPen);
    ui->customPlot->graph(0)->setBrush(QBrush(QPixmap(":/image/bg2.jpg"))); // fill with texture of specified image

    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));

    // activate channel fill for graph 0 towards graph 1:
    ui->customPlot->graph(0)->setChannelFillGraph(ui->customPlot->graph(1));

    // generate data:
    QVector<double> x(250);
    QVector<double> y0(250), y1(250);
    for (int i = 0; i < 250; ++i) {
        // just playing with numbers, not much to learn here
        x[i] = 3 * i / 250.0;
        y0[i] = 1 + qExp(-x[i] * x[i] * 0.8) * (x[i] * x[i] + x[i]);
        y1[i] = 1 - qExp(-x[i] * x[i] * 0.4) * (x[i] * x[i]) * 0.1;
    }

    // pass data points to graphs:
    ui->customPlot->graph(0)->setData(x, y0);
    ui->customPlot->graph(1)->setData(x, y1);
    // activate top and right axes, which are invisible by default:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    // make tick labels invisible on top and right axis:
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);
    // set ranges:
    ui->customPlot->xAxis->setRange(0, 2.5);
    ui->customPlot->yAxis->setRange(0.9, 1.6);
    // assign top/right axes same properties as bottom/left:
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
