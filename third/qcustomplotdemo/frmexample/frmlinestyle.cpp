#include "frmlinestyle.h"
#include "ui_frmlinestyle.h"
#include "qdebug.h"

frmLineStyle::frmLineStyle(QWidget *parent) : QWidget(parent), ui(new Ui::frmLineStyle)
{
    ui->setupUi(this);
    this->initForm();
}

frmLineStyle::~frmLineStyle()
{
    delete ui;
}

void frmLineStyle::initForm()
{
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(QFont("Helvetica", 9));

    QPen pen;
    QStringList lineNames;
    lineNames << "lsNone" << "lsLine" << "lsStepLeft" << "lsStepRight" << "lsStepCenter" << "lsImpulse";

    // add graphs with different line styles:
    for (int i = QCPGraph::lsNone; i <= QCPGraph::lsImpulse; ++i) {
        ui->customPlot->addGraph();
        pen.setColor(QColor(qSin(i * 1 + 1.2) * 80 + 80, qSin(i * 0.3 + 0) * 80 + 80, qSin(i * 0.3 + 1.5) * 80 + 80));
        ui->customPlot->graph()->setPen(pen);
        ui->customPlot->graph()->setName(lineNames.at(i - QCPGraph::lsNone));
        ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)i);
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

        // generate data:
        QVector<double> x(15), y(15);
        for (int j = 0; j < 15; ++j) {
            x[j] = j / 15.0 * 5 * 3.14 + 0.01;
            y[j] = 7 * qSin(x[j]) / x[j] - (i - QCPGraph::lsNone) * 5 + (QCPGraph::lsImpulse) * 5 + 2;
        }

        ui->customPlot->graph()->setData(x, y);
        ui->customPlot->graph()->rescaleAxes(true);
    }

    // zoom out a bit:
    ui->customPlot->yAxis->scaleRange(1.1, ui->customPlot->yAxis->range().center());
    ui->customPlot->xAxis->scaleRange(1.1, ui->customPlot->xAxis->range().center());

    // set blank axis lines:
    ui->customPlot->xAxis->setTicks(false);
    ui->customPlot->yAxis->setTicks(true);
    ui->customPlot->xAxis->setTickLabels(false);
    ui->customPlot->yAxis->setTickLabels(true);

    // make top right axes clones of bottom left axes:
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
