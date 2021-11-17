#include "circularbuffer.h"
#include <math.h>

CircularBuffer::CircularBuffer( double interval, size_t numPoints ):
    d_y( NULL ),
    d_referenceTime( 0.0 ),
    d_startIndex( 0 ),
    d_offset( 0.0 )
{
    fill( interval, numPoints );
}

void CircularBuffer::fill( double interval, size_t numPoints )
{
    if ( interval <= 0.0 || numPoints < 2 )
        return;

    d_values.resize( numPoints );
    d_values.fill( 0.0 );

    if ( d_y )
    {
        d_step = interval / ( numPoints - 2 );
        for ( size_t i = 0; i < numPoints; i++ )
            d_values[i] = d_y( i * d_step );
    }

    d_interval = interval;
}

void CircularBuffer::setFunction( double( *y )( double ) )
{
    d_y = y;
}

void CircularBuffer::setReferenceTime( double timeStamp )
{
    d_referenceTime = timeStamp;

    const double startTime = ::fmod( d_referenceTime, d_values.size() * d_step );

    d_startIndex = int( startTime / d_step ); // floor
    d_offset = ::fmod( startTime, d_step );
}

double CircularBuffer::referenceTime() const
{
    return d_referenceTime;
}

size_t CircularBuffer::size() const
{
    return d_values.size();
}

QPointF CircularBuffer::sample( size_t i ) const
{
    const int size = d_values.size();

    int index = d_startIndex + i;
    if ( index >= size )
        index -= size;

    const double x = i * d_step - d_offset - d_interval;
    const double y = d_values.data()[index];

    return QPointF( x, y );
}

QRectF CircularBuffer::boundingRect() const
{
    return QRectF( -1.0, -d_interval, 2.0, d_interval );
}