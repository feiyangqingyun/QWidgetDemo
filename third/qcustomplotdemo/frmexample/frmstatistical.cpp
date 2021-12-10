#include "frmstatistical.h"
#include "ui_frmstatistical.h"
#include "qdebug.h"

frmStatistical::frmStatistical(QWidget *parent) : QWidget(parent), ui(new Ui::frmStatistical)
{
    ui->setupUi(this);
    this->initForm();
}

frmStatistical::~frmStatistical()
{
    delete ui;
}

void frmStatistical::initForm()
{
    QCPStatisticalBox *statistical = new QCPStatisticalBox(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QBrush boxBrush(QColor(60, 60, 255, 100));
    boxBrush.setStyle(Qt::Dense6Pattern); // make it look oldschool
    statistical->setBrush(boxBrush);

    // specify data:
    statistical->addData(1, 1.1, 1.9, 2.25, 2.7, 4.2);
    statistical->addData(2, 0.8, 1.6, 2.2, 3.2, 4.9, QVector<double>() << 0.7 << 0.34 << 0.45 << 6.2 << 5.84); // provide some outliers as QVector
    statistical->addData(3, 0.2, 0.7, 1.1, 1.6, 2.9);

    // prepare manual x axis labels:
    ui->customPlot->xAxis->setSubTicks(false);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->setTickLabelRotation(20);
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, "Sample 1");
    textTicker->addTick(2, "Sample 2");
    textTicker->addTick(3, "Control Group");
    ui->customPlot->xAxis->setTicker(textTicker);

    // prepare axes:
    ui->customPlot->yAxis->setLabel(QString::fromUtf8("O? Absorption [mg]"));
    ui->customPlot->rescaleAxes();
    ui->customPlot->xAxis->scaleRange(1.7, ui->customPlot->xAxis->range().center());
    ui->customPlot->yAxis->setRange(0, 7);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
