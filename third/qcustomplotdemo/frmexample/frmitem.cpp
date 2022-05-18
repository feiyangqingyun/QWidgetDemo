#include "frmitem.h"
#include "ui_frmitem.h"
#include "qdebug.h"

frmItem::frmItem(QWidget *parent) : QWidget(parent), ui(new Ui::frmItem)
{
    ui->setupUi(this);
    this->initForm();
}

frmItem::~frmItem()
{
    delete ui;
}

void frmItem::showEvent(QShowEvent *)
{
    dataTimer.start(0);
}

void frmItem::hideEvent(QHideEvent *)
{
    dataTimer.stop();
}

void frmItem::initForm()
{
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph = ui->customPlot->addGraph();
    int n = 500;
    double phase = 0;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i / (double)(n - 1) * 34 - 17;
        y[i] = qExp(-x[i] * x[i] / 20.0) * qSin(k * x[i] + phase);
    }

    graph->setData(x, y);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();
    ui->customPlot->yAxis->setRange(-1.45, 1.65);
    ui->customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    // add the bracket at the top:
    QCPItemBracket *bracket = new QCPItemBracket(ui->customPlot);
    bracket->left->setCoords(-8, 1.1);
    bracket->right->setCoords(8, 1.1);
    bracket->setLength(13);

    // add the text label at the top:
    QCPItemText *wavePacketText = new QCPItemText(ui->customPlot);
    wavePacketText->position->setParentAnchor(bracket->center);
    wavePacketText->position->setCoords(0, -10); // move 10 pixels to the top from bracket center anchor
    wavePacketText->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    wavePacketText->setText("Wavepacket");
    wavePacketText->setFont(QFont(font().family(), 10));

    // add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):
    phaseTracer = new QCPItemTracer(ui->customPlot);
    phaseTracer->setGraph(graph);
    phaseTracer->setGraphKey((M_PI * 1.5 - phase) / k);
    phaseTracer->setInterpolating(true);
    phaseTracer->setStyle(QCPItemTracer::tsCircle);
    phaseTracer->setPen(QPen(Qt::red));
    phaseTracer->setBrush(Qt::red);
    phaseTracer->setSize(7);

    // add label for phase tracer:
    QCPItemText *phaseTracerText = new QCPItemText(ui->customPlot);
    phaseTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    phaseTracerText->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
    phaseTracerText->position->setCoords(1.0, 0.95); // lower right corner of axis rect
    phaseTracerText->setText("Points of fixed\nphase define\nphase velocity vp");
    phaseTracerText->setTextAlignment(Qt::AlignLeft);
    phaseTracerText->setFont(QFont(font().family(), 9));
    phaseTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at phase tracer, coming from label:
    QCPItemCurve *phaseTracerArrow = new QCPItemCurve(ui->customPlot);
    phaseTracerArrow->start->setParentAnchor(phaseTracerText->left);
    phaseTracerArrow->startDir->setParentAnchor(phaseTracerArrow->start);
    phaseTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    phaseTracerArrow->end->setParentAnchor(phaseTracer->position);
    phaseTracerArrow->end->setCoords(10, 10);
    phaseTracerArrow->endDir->setParentAnchor(phaseTracerArrow->end);
    phaseTracerArrow->endDir->setCoords(30, 30);
    phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (phaseTracerText->bottom->pixelPosition().y() - phaseTracerText->top->pixelPosition().y()) * 0.85));

    // add the group velocity tracer (green circle):
    QCPItemTracer *groupTracer = new QCPItemTracer(ui->customPlot);
    groupTracer->setGraph(graph);
    groupTracer->setGraphKey(5.5);
    groupTracer->setInterpolating(true);
    groupTracer->setStyle(QCPItemTracer::tsCircle);
    groupTracer->setPen(QPen(Qt::green));
    groupTracer->setBrush(Qt::green);
    groupTracer->setSize(7);

    // add label for group tracer:
    QCPItemText *groupTracerText = new QCPItemText(ui->customPlot);
    groupTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    groupTracerText->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
    groupTracerText->position->setCoords(1.0, 0.20); // lower right corner of axis rect
    groupTracerText->setText("Fixed positions in\nwave packet define\ngroup velocity vg");
    groupTracerText->setTextAlignment(Qt::AlignLeft);
    groupTracerText->setFont(QFont(font().family(), 9));
    groupTracerText->setPadding(QMargins(8, 0, 0, 0));

    // add arrow pointing at group tracer, coming from label:
    QCPItemCurve *groupTracerArrow = new QCPItemCurve(ui->customPlot);
    groupTracerArrow->start->setParentAnchor(groupTracerText->left);
    groupTracerArrow->startDir->setParentAnchor(groupTracerArrow->start);
    groupTracerArrow->startDir->setCoords(-40, 0); // direction 30 pixels to the left of parent anchor (tracerArrow->start)
    groupTracerArrow->end->setCoords(5.5, 0.4);
    groupTracerArrow->endDir->setParentAnchor(groupTracerArrow->end);
    groupTracerArrow->endDir->setCoords(0, -40);
    groupTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
    groupTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (groupTracerText->bottom->pixelPosition().y() - groupTracerText->top->pixelPosition().y()) * 0.85));

    // add dispersion arrow:
    QCPItemCurve *arrow = new QCPItemCurve(ui->customPlot);
    arrow->start->setCoords(1, -1.1);
    arrow->startDir->setCoords(-1, -1.3);
    arrow->endDir->setCoords(-5, -0.3);
    arrow->end->setCoords(-10, -0.2);
    arrow->setHead(QCPLineEnding::esSpikeArrow);

    // add the dispersion arrow label:
    QCPItemText *dispersionText = new QCPItemText(ui->customPlot);
    dispersionText->position->setCoords(-6, -0.9);
    dispersionText->setRotation(40);
    dispersionText->setText("Dispersion with\nvp < vg");
    dispersionText->setFont(QFont(font().family(), 10));

    // setup a timer that repeatedly calls MainWindow::bracketDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(bracketDataSlot()));    
}

void frmItem::bracketDataSlot()
{
    double secs = QCPAxisTickerDateTime::dateTimeToKey(QDateTime::currentDateTime());

    // update data to make phase move:
    int n = 500;
    double phase = secs * 5;
    double k = 3;
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i / (double)(n - 1) * 34 - 17;
        y[i] = qExp(-x[i] * x[i] / 20.0) * qSin(k * x[i] + phase);
    }

    ui->customPlot->graph()->setData(x, y);
    phaseTracer->setGraphKey((8 * M_PI + fmod(M_PI * 1.5 - phase, 6 * M_PI)) / k);
    ui->customPlot->replot();

    // calculate frames per second:
    double key = secs;
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key - lastFpsKey > 2) { // average fps over 2 seconds
//        ui->statusBar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
//            .arg(ui->customPlot->graph(0)->data()->size())
//            , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}
