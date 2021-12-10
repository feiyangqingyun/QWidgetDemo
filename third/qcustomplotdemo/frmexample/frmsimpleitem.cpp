#include "frmsimpleitem.h"
#include "ui_frmsimpleitem.h"
#include "qdebug.h"

frmSimpleItem::frmSimpleItem(QWidget *parent) : QWidget(parent), ui(new Ui::frmSimpleItem)
{
    ui->setupUi(this);
    this->initForm();
}

frmSimpleItem::~frmSimpleItem()
{
    delete ui;
}

void frmSimpleItem::initForm()
{
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // add the text label at the top:
    QCPItemText *textLabel = new QCPItemText(ui->customPlot);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
    textLabel->setText("Text Item Demo");
    textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
    textLabel->setPen(QPen(Qt::black)); // show black border around text

    // add the arrow:
    QCPItemLine *arrow = new QCPItemLine(ui->customPlot);
    arrow->start->setParentAnchor(textLabel->bottom);
    arrow->end->setCoords(4, 1.6); // point to (4, 1.6) in x-y-plot coordinates
    arrow->setHead(QCPLineEnding::esSpikeArrow);
}
