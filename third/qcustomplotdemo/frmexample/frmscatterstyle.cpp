#include "frmscatterstyle.h"
#include "ui_frmscatterstyle.h"
#include "qmetaobject.h"
#include "qdebug.h"

frmScatterStyle::frmScatterStyle(QWidget *parent) : QWidget(parent), ui(new Ui::frmScatterStyle)
{
    ui->setupUi(this);
    this->initForm();
}

frmScatterStyle::~frmScatterStyle()
{
    delete ui;
}

void frmScatterStyle::initForm()
{
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(QFont("Helvetica", 9));
    ui->customPlot->legend->setRowSpacing(-3);

    QVector<QCPScatterStyle::ScatterShape> shapes;
    shapes << QCPScatterStyle::ssCross;
    shapes << QCPScatterStyle::ssPlus;
    shapes << QCPScatterStyle::ssCircle;
    shapes << QCPScatterStyle::ssDisc;
    shapes << QCPScatterStyle::ssSquare;
    shapes << QCPScatterStyle::ssDiamond;
    shapes << QCPScatterStyle::ssStar;
    shapes << QCPScatterStyle::ssTriangle;
    shapes << QCPScatterStyle::ssTriangleInverted;
    shapes << QCPScatterStyle::ssCrossSquare;
    shapes << QCPScatterStyle::ssPlusSquare;
    shapes << QCPScatterStyle::ssCrossCircle;
    shapes << QCPScatterStyle::ssPlusCircle;
    shapes << QCPScatterStyle::ssPeace;
    shapes << QCPScatterStyle::ssCustom;

    QPen pen;
    // add graphs with different scatter styles:
    for (int i = 0; i < shapes.size(); ++i) {
        ui->customPlot->addGraph();
        pen.setColor(QColor(qSin(i * 0.3) * 100 + 100, qSin(i * 0.6 + 0.7) * 100 + 100, qSin(i * 0.4 + 0.6) * 100 + 100));
        // generate data:
        QVector<double> x(10), y(10);
        for (int k = 0; k < 10; ++k) {
            x[k] = k / 10.0 * 4 * 3.14 + 0.01;
            y[k] = 7 * qSin(x[k]) / x[k] + (shapes.size() - i) * 5;
        }
        ui->customPlot->graph()->setData(x, y);
        ui->customPlot->graph()->rescaleAxes(true);
        ui->customPlot->graph()->setPen(pen);

        //枚举值转字符串
        QMetaObject metaObject = QCPScatterStyle::staticMetaObject;
        QMetaEnum metaEnum = metaObject.enumerator(metaObject.indexOfEnumerator("ScatterShape"));
        QString name = metaEnum.valueToKey(shapes.at(i));
        ui->customPlot->graph()->setName(name);

        ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
        // set scatter style:
        if (shapes.at(i) != QCPScatterStyle::ssCustom) {
            ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes.at(i), 10));
        } else {
            QPainterPath customScatterPath;
            for (int i = 0; i < 3; ++i) {
                customScatterPath.cubicTo(qCos(2 * M_PI * i / 3.0) * 9, qSin(2 * M_PI * i / 3.0) * 9, qCos(2 * M_PI * (i + 0.9) / 3.0) * 9, qSin(2 * M_PI * (i + 0.9) / 3.0) * 9, 0, 0);
            }
            ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(customScatterPath, QPen(Qt::black, 0), QColor(40, 70, 255, 50), 10));
        }
    }

    // set blank axis lines:
    ui->customPlot->rescaleAxes();
    ui->customPlot->xAxis->setTicks(false);
    ui->customPlot->yAxis->setTicks(false);
    ui->customPlot->xAxis->setTickLabels(false);
    ui->customPlot->yAxis->setTickLabels(false);
    // make top right axes clones of bottom left axes:
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
