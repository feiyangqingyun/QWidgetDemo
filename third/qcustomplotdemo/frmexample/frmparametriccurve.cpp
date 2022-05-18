#include "frmparametriccurve.h"
#include "ui_frmparametriccurve.h"
#include "qdebug.h"

frmParametricCurve::frmParametricCurve(QWidget *parent) : QWidget(parent), ui(new Ui::frmParametricCurve)
{
    ui->setupUi(this);
    this->initForm();
}

frmParametricCurve::~frmParametricCurve()
{
    delete ui;
}

void frmParametricCurve::initForm()
{
    // create empty curve objects:
    QCPCurve *fermatSpiral1 = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QCPCurve *fermatSpiral2 = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QCPCurve *deltoidRadial = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);

    // generate the curve data points:
    const int pointCount = 500;
    QVector<QCPCurveData> dataSpiral1(pointCount), dataSpiral2(pointCount), dataDeltoid(pointCount);
    for (int i = 0; i < pointCount; ++i) {
        double phi = i / (double)(pointCount - 1) * 8 * M_PI;
        double theta = i / (double)(pointCount - 1) * 2 * M_PI;
        dataSpiral1[i] = QCPCurveData(i, qSqrt(phi) * qCos(phi), qSqrt(phi) * qSin(phi));
        dataSpiral2[i] = QCPCurveData(i, -dataSpiral1[i].key, -dataSpiral1[i].value);
        dataDeltoid[i] = QCPCurveData(i, 2 * qCos(2 * theta) + qCos(1 * theta) + 2 * qSin(theta), 2 * qSin(2 * theta) - qSin(1 * theta));
    }

    // pass the data to the curves; we know t (i in loop above) is ascending, so set alreadySorted=true (saves an extra internal sort):
    fermatSpiral1->data()->set(dataSpiral1, true);
    fermatSpiral2->data()->set(dataSpiral2, true);
    deltoidRadial->data()->set(dataDeltoid, true);

    // color the curves:
    fermatSpiral1->setPen(QPen(Qt::blue));
    fermatSpiral1->setBrush(QBrush(QColor(0, 0, 255, 20)));
    fermatSpiral2->setPen(QPen(QColor(255, 120, 0)));
    fermatSpiral2->setBrush(QBrush(QColor(255, 120, 0, 30)));

    QRadialGradient radialGrad(QPointF(310, 180), 200);
    radialGrad.setColorAt(0, QColor(170, 20, 240, 100));
    radialGrad.setColorAt(0.5, QColor(20, 10, 255, 40));
    radialGrad.setColorAt(1, QColor(120, 20, 240, 10));
    deltoidRadial->setPen(QPen(QColor(170, 20, 240)));
    deltoidRadial->setBrush(QBrush(radialGrad));

    // set some basic ui->customPlot config:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->rescaleAxes();
}
