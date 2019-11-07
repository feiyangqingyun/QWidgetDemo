#include "curvedata.h"
#include "signaldata.h"

const SignalData &CurveData::values() const
{
    return SignalData::instance();
}

SignalData &CurveData::values()
{
    return SignalData::instance();
}

QPointF CurveData::sample( size_t i ) const
{
    return SignalData::instance().value( i );
}

size_t CurveData::size() const
{
    return SignalData::instance().size();
}

QRectF CurveData::boundingRect() const
{
    return SignalData::instance().boundingRect();
}
