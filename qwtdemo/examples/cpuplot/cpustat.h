#include <qdatetime.h>

class CpuStat
{
public:
    CpuStat();
    void statistic( double &user, double &system );
    QTime upTime() const;

    enum Value
    {
        User,
        Nice,
        System,
        Idle,

        NValues
    };

private:
    void lookUp( double[NValues] ) const;
    double procValues[NValues];
};
