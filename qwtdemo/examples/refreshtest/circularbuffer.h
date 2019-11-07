#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <qwt_series_data.h>
#include <qvector.h>

class CircularBuffer: public QwtSeriesData<QPointF>
{
public:
    CircularBuffer( double interval = 10.0, size_t numPoints = 1000 );
    void fill( double interval, size_t numPoints );

    void setReferenceTime( double );
    double referenceTime() const;

    virtual size_t size() const;
    virtual QPointF sample( size_t i ) const;

    virtual QRectF boundingRect() const;

    void setFunction( double( *y )( double ) );

private:
    double ( *d_y )( double );

    double d_referenceTime;
    double d_interval;
    QVector<double> d_values;

    double d_step;
    int d_startIndex;
    double d_offset;
};

#endif
