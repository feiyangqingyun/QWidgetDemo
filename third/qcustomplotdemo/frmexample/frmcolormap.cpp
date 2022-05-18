#include "frmcolormap.h"
#include "ui_frmcolormap.h"
#include "qdebug.h"

frmColorMap::frmColorMap(QWidget *parent) : QWidget(parent), ui(new Ui::frmColorMap)
{
    ui->setupUi(this);
    this->initForm();
}

frmColorMap::~frmColorMap()
{
    delete ui;
}

void frmColorMap::initForm()
{
    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    int nx = 200;
    int ny = 200;
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    // now we assign some data, by accessing the QCPColorMapData instance of the color map:
    double x, y, z;
    for (int xIndex = 0; xIndex < nx; ++xIndex) {
        for (int yIndex = 0; yIndex < ny; ++yIndex) {
            colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
            double r = 3 * qSqrt(x * x + y * y) + 1e-2;
            z = 2 * x * (qCos(r + 2) / r - qSin(r + 2) / r); // the B field strength of dipole radiation (modulo physical constants)
            colorMap->data()->setCell(xIndex, yIndex, z);
        }
    }

    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
}
