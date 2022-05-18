#include "frmmultiaxis.h"
#include "ui_frmmultiaxis.h"
#include "qdebug.h"

frmMultiAxis::frmMultiAxis(QWidget *parent) : QWidget(parent), ui(new Ui::frmMultiAxis)
{
    ui->setupUi(this);
    this->initForm();
}

frmMultiAxis::~frmMultiAxis()
{
    delete ui;
}

void frmMultiAxis::initForm()
{
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    // setup for graph 0: key axis left, value axis bottom
    // will contain left maxwell-like function
    ui->customPlot->addGraph(ui->customPlot->yAxis, ui->customPlot->xAxis);
    ui->customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
    ui->customPlot->graph(0)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // fill with texture of specified image
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    ui->customPlot->graph(0)->setName("Left maxwell function");

    // setup for graph 1: key axis bottom, value axis left (those are the default axes)
    // will contain bottom maxwell-like function with error bars
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // same fill as we used for graph 0
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
    ui->customPlot->graph(1)->setName("Bottom maxwell function");

    QCPErrorBars *errorBars = new QCPErrorBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    errorBars->removeFromLegend();
    errorBars->setDataPlottable(ui->customPlot->graph(1));

    // setup for graph 2: key axis top, value axis right
    // will contain high frequency sine with low frequency beating:
    ui->customPlot->addGraph(ui->customPlot->xAxis2, ui->customPlot->yAxis2);
    ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(2)->setName("High frequency sine");

    // setup for graph 3: same axes as graph 2
    // will contain low frequency beating envelope of graph 2
    ui->customPlot->addGraph(ui->customPlot->xAxis2, ui->customPlot->yAxis2);
    QPen blueDotPen;
    blueDotPen.setColor(QColor(30, 40, 255, 150));
    blueDotPen.setStyle(Qt::DotLine);
    blueDotPen.setWidthF(4);
    ui->customPlot->graph(3)->setPen(blueDotPen);
    ui->customPlot->graph(3)->setName("Sine envelope");

    // setup for graph 4: key axis right, value axis top
    // will contain parabolically distributed data points with some random perturbance
    ui->customPlot->addGraph(ui->customPlot->yAxis2, ui->customPlot->xAxis2);
    ui->customPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
    ui->customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    ui->customPlot->graph(4)->setName("Some random data around\na quadratic function");

    // generate data, just playing with numbers, not much to learn here:
    QVector<double> x0(25), y0(25);
    QVector<double> x1(15), y1(15), y1err(15);
    QVector<double> x2(250), y2(250);
    QVector<double> x3(250), y3(250);
    QVector<double> x4(250), y4(250);

    for (int i = 0; i < 25; ++i) { // data for graph 0
        x0[i] = 3 * i / 25.0;
        y0[i] = qExp(-x0[i] * x0[i] * 0.8) * (x0[i] * x0[i] + x0[i]);
    }

    for (int i = 0; i < 15; ++i) { // data for graph 1
        x1[i] = 3 * i / 15.0;;
        y1[i] = qExp(-x1[i] * x1[i]) * (x1[i] * x1[i]) * 2.6;
        y1err[i] = y1[i] * 0.25;
    }

    for (int i = 0; i < 250; ++i) { // data for graphs 2, 3 and 4
        x2[i] = i / 250.0 * 3 * M_PI;
        x3[i] = x2[i];
        x4[i] = i / 250.0 * 100 - 50;
        y2[i] = qSin(x2[i] * 12) * qCos(x2[i]) * 10;
        y3[i] = qCos(x3[i]) * 10;
        y4[i] = 0.01 * x4[i] * x4[i] + 1.5 * (rand() / (double)RAND_MAX - 0.5) + 1.5 * M_PI;
    }

    // pass data points to graphs:
    ui->customPlot->graph(0)->setData(x0, y0);
    ui->customPlot->graph(1)->setData(x1, y1);
    errorBars->setData(y1err);
    ui->customPlot->graph(2)->setData(x2, y2);
    ui->customPlot->graph(3)->setData(x3, y3);
    ui->customPlot->graph(4)->setData(x4, y4);

    // activate top and right axes, which are invisible by default:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);

    // set ranges appropriate to show data:
    ui->customPlot->xAxis->setRange(0, 2.7);
    ui->customPlot->yAxis->setRange(0, 2.6);
    ui->customPlot->xAxis2->setRange(0, 3.0 * M_PI);
    ui->customPlot->yAxis2->setRange(-70, 35);

    // set pi ticks on top axis:
    ui->customPlot->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
    // add title layout element:
    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Way too many graphs in one plot", QFont("sans", 12, QFont::Bold)));

    // set labels:
    ui->customPlot->xAxis->setLabel("Bottom axis with outward ticks");
    ui->customPlot->yAxis->setLabel("Left axis label");
    ui->customPlot->xAxis2->setLabel("Top axis label");
    ui->customPlot->yAxis2->setLabel("Right axis label");
    // make ticks on bottom axis go outward:
    ui->customPlot->xAxis->setTickLength(0, 5);
    ui->customPlot->xAxis->setSubTickLength(0, 3);
    // make ticks on right axis go inward and outward:
    ui->customPlot->yAxis2->setTickLength(3, 3);
    ui->customPlot->yAxis2->setSubTickLength(1, 1);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
