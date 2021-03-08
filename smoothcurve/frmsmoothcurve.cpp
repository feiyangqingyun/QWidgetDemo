#include "frmsmoothcurve.h"
#include "ui_frmsmoothcurve.h"
#include "smoothcurve.h"
#include "qpainter.h"
#include "qdatetime.h"
#include "qdebug.h"

frmSmoothCurve::frmSmoothCurve(QWidget *parent) : QWidget(parent), ui(new Ui::frmSmoothCurve)
{
    ui->setupUi(this);

    //初始化随机数种子
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    //随机生成曲线上的点
    int x = -300;
    while (x < 300) {
        datas << QPointF(x, qrand() % 300 - 100);
        x += qMin(qrand() % 30 + 5, 300);
    }

    //根据曲线上的点创建平滑曲线
    smoothCurve = SmoothCurve::createSmoothCurve(datas);

    // 直接连接点的创建非平滑曲线曲线
    smoothCurve2.moveTo(datas[0]);
    for (int i = 1; i < datas.size(); ++i) {
        smoothCurve2.lineTo(datas[i]);
    }

    connect(ui->showKnotsCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->showSmoothCurveCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
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

    //被选中时显示平滑曲线,否则显示非平滑曲线
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    if (ui->showSmoothCurveCheckBox->isChecked()) {
        painter.drawPath(smoothCurve);
    } else {
        painter.drawPath(smoothCurve2);
    }

    //如果曲线上的点可见则显示出来
    if (ui->showKnotsCheckBox->isChecked()) {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::gray);
        foreach (QPointF p, datas) {
            painter.drawEllipse(p, 3, 3);
        }
    }
}
