#ifndef SMOOTHCURVE_H
#define SMOOTHCURVE_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QPainterPath>

#ifdef quc
class Q_DECL_EXPORT SmoothCurve
#else
class SmoothCurve
#endif

{
public:
    //创建平滑曲线路径
    static QPainterPath createSmoothCurve(const QVector<QPointF> &points);
    static QPainterPath createSmoothCurve2(const QVector<QPointF> &points);

private:
    static void calculateFirstControlPoints(double *&result, const double *rhs, int n);
    static void calculateControlPoints(const QVector<QPointF> &datas,
                                       QVector<QPointF> *firstControlPoints,
                                       QVector<QPointF> *secondControlPoints);
};

#endif // SMOOTHCURVE_H
