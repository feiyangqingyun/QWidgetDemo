#include "frmpolarplot.h"
#include "ui_frmpolarplot.h"
#include "qdebug.h"

frmPolarPlot::frmPolarPlot(QWidget *parent) : QWidget(parent), ui(new Ui::frmPolarPlot)
{
    ui->setupUi(this);
    this->initForm();
}

frmPolarPlot::~frmPolarPlot()
{
    delete ui;
}

void frmPolarPlot::initForm()
{
    //南丁格尔图示 2.1 版本新增加的
#ifdef qcustomplot_v2_1
    // Warning: Polar plots are a still a tech preview

    ui->customPlot->plotLayout()->clear();
    QCPPolarAxisAngular *angularAxis = new QCPPolarAxisAngular(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 0, angularAxis);
    /* This is how we could set the angular axis to show pi symbols instead of degree numbers:
    QSharedPointer<QCPAxisTickerPi> ticker(new QCPAxisTickerPi);
    ticker->setPiValue(180);
    ticker->setTickCount(8);
    polarAxis->setTicker(ticker);
    */

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    angularAxis->setRangeDrag(false);
    angularAxis->setTickLabelMode(QCPPolarAxisAngular::lmUpright);

    angularAxis->radialAxis()->setTickLabelMode(QCPPolarAxisRadial::lmUpright);
    angularAxis->radialAxis()->setTickLabelRotation(0);
    angularAxis->radialAxis()->setAngle(45);

    angularAxis->grid()->setAngularPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));
    angularAxis->grid()->setSubGridType(QCPPolarGrid::gtAll);

    QCPPolarGraph *g1 = new QCPPolarGraph(angularAxis, angularAxis->radialAxis());
    QCPPolarGraph *g2 = new QCPPolarGraph(angularAxis, angularAxis->radialAxis());
    g2->setPen(QPen(QColor(255, 150, 20)));
    g2->setBrush(QColor(255, 150, 20, 50));
    g1->setScatterStyle(QCPScatterStyle::ssDisc);

    for (int i = 0; i < 100; ++i) {
        g1->addData(i / 100.0 * 360.0, qSin(i / 100.0 * M_PI * 8) * 8 + 1);
        g2->addData(i / 100.0 * 360.0, qSin(i / 100.0 * M_PI * 6) * 2);
    }

    angularAxis->setRange(0, 360);
    angularAxis->radialAxis()->setRange(-10, 10);
#endif
}
