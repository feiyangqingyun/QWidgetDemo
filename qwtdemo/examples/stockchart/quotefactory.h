#ifndef _QUOTE_FACTORY_H_
#define _QUOTE_FACTORY_H_

#include <qwt_series_data.h>

class QuoteFactory
{
public:
    enum Stock
    {
        BMW,
        Daimler,
        Porsche,

        NumStocks
    };

    static QVector<QwtOHLCSample> samples2010( Stock );
    static QString title( Stock );
};

#endif
