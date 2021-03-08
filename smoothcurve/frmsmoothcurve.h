#ifndef FRMSMOOTHCURVE_H
#define FRMSMOOTHCURVE_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include <QPainterPath>

namespace Ui {
class frmSmoothCurve;
}

class frmSmoothCurve : public QWidget
{
    Q_OBJECT

public:
    explicit frmSmoothCurve(QWidget *parent = 0);
    ~frmSmoothCurve();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::frmSmoothCurve *ui;
    QVector<QPointF> datas;     //曲线上的点
    QPainterPath smoothCurve;   //平滑曲线
    QPainterPath smoothCurve2;  //直接连接点的非平滑曲线
};

#endif // FRMSMOOTHCURVE_H
