#ifndef _FRIEDBERG_2007_H_
#define _FRIEDBERG_2007_H_

class Temperature
{
public:
    Temperature():
        minValue( 0.0 ),
        maxValue( 0.0 ),
        averageValue( 0.0 )
    {
    }

    Temperature( double min, double max, double average ):
        minValue( min ),
        maxValue( max ),
        averageValue( average )
    {
    }

    double minValue;
    double maxValue;
    double averageValue;
};

extern Temperature friedberg2007[];

#endif
