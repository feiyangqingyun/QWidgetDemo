#include "frmsmoothcurve.h"
#include "ui_frmsmoothcurve.h"
#include "smoothcurve.h"
#include "qpainter.h"
#include "qdatetime.h"
#include "qdebug.h"

#define TIMEMS QTime::currentTime().toString("hh:mm:ss zzz")

frmSmoothCurve::frmSmoothCurve(QWidget *parent) : QWidget(parent), ui(new Ui::frmSmoothCurve)
{
    ui->setupUi(this);

    //初始化随机数种子
    srand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    //随机生成曲线上的点
    int x = -300;
    while (x < 300) {
        datas << QPointF(x, rand() % 300 - 100);
        x += qMin(rand() % 30 + 5, 300);
    }

    //正常曲线
    pathNormal.moveTo(datas.at(0));
    for (int i = 1; i < datas.size(); ++i) {
        pathNormal.lineTo(datas.at(i));
    }

    //平滑曲线1
    //qDebug() << TIMEMS << "createSmoothCurve start";
    pathSmooth1 = SmoothCurve::createSmoothCurve(datas);
    //qDebug() << TIMEMS << "createSmoothCurve stop";

    //平滑曲线2
    //qDebug() << TIMEMS << "createSmoothCurve2 start";
    pathSmooth2 = SmoothCurve::createSmoothCurve2(datas);
    //qDebug() << TIMEMS << "createSmoothCurve2 stop";

    ui->ckShowPoint->setChecked(true);
    connect(ui->ckShowPoint, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->rbtnPathNormal, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->rbtnPathSmooth1, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->rbtnPathSmooth2, SIGNAL(clicked(bool)), this, SLOT(update()));
}

frmSmoothCurve::~frmSmoothCurve()
{
    delete ui;
}

void frmSmoothCurve::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(1, -1);

    //画坐标轴
    painter.setPen(QColor(180, 180, 180));
    painter.drawLine(-250, 0, 250, 0);
    painter.drawLine(0, 150, 0, -150);

    //根据选择绘制不同的曲线路径
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    if (ui->rbtnPathSmooth1->isChecked()) {
        painter.drawPath(pathSmooth1);
    } else if (ui->rbtnPathSmooth2->isChecked()) {
        painter.drawPath(pathSmooth2);
    } else {
        painter.drawPath(pathNormal);
    }

    //如果曲线上的点可见则显示出来
    if (ui->ckShowPoint->isChecked()) {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::gray);
        foreach (QPointF point, datas) {
            painter.drawEllipse(point, 3, 3);
        }
    }
}
