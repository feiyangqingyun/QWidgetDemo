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
    QPainterPath pathNormal;    //正常曲线
    QPainterPath pathSmooth1;   //平滑曲线1
    QPainterPath pathSmooth2;   //平滑曲线2
};

#endif // FRMSMOOTHCURVE_H
