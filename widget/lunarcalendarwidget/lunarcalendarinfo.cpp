#pragma execution_character_set("utf-8")

#include "lunarcalendarinfo.h"
#include "qmutex.h"
#include "qdebug.h"

#define year_2099

QScopedPointer<LunarCalendarInfo> LunarCalendarInfo::self;
LunarCalendarInfo *LunarCalendarInfo::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new LunarCalendarInfo);
        }
    }

    return self.data();
}

LunarCalendarInfo::LunarCalendarInfo(QObject *parent) : QObject(parent)
{
    //农历查表
    lunarCalendarTable << 0x04AE53 << 0x0A5748 << 0x5526BD << 0x0D2650 << 0x0D9544 << 0x46AAB9 << 0x056A4D << 0x09AD42 << 0x24AEB6 << 0x04AE4A; //1901-1910
    lunarCalendarTable << 0x6A4DBE << 0x0A4D52 << 0x0D2546 << 0x5D52BA << 0x0B544E << 0x0D6A43 << 0x296D37 << 0x095B4B << 0x749BC1 << 0x049754; //1911-1920
    lunarCalendarTable << 0x0A4B48 << 0x5B25BC << 0x06A550 << 0x06D445 << 0x4ADAB8 << 0x02B64D << 0x095742 << 0x2497B7 << 0x04974A << 0x664B3E; //1921-1930
    lunarCalendarTable << 0x0D4A51 << 0x0EA546 << 0x56D4BA << 0x05AD4E << 0x02B644 << 0x393738 << 0x092E4B << 0x7C96BF << 0x0C9553 << 0x0D4A48; //1931-1940
    lunarCalendarTable << 0x6DA53B << 0x0B554F << 0x056A45 << 0x4AADB9 << 0x025D4D << 0x092D42 << 0x2C95B6 << 0x0A954A << 0x7B4ABD << 0x06CA51; //1941-1950
    lunarCalendarTable << 0x0B5546 << 0x555ABB << 0x04DA4E << 0x0A5B43 << 0x352BB8 << 0x052B4C << 0x8A953F << 0x0E9552 << 0x06AA48 << 0x6AD53C; //1951-1960
    lunarCalendarTable << 0x0AB54F << 0x04B645 << 0x4A5739 << 0x0A574D << 0x052642 << 0x3E9335 << 0x0D9549 << 0x75AABE << 0x056A51 << 0x096D46; //1961-1970
    lunarCalendarTable << 0x54AEBB << 0x04AD4F << 0x0A4D43 << 0x4D26B7 << 0x0D254B << 0x8D52BF << 0x0B5452 << 0x0B6A47 << 0x696D3C << 0x095B50; //1971-1980
    lunarCalendarTable << 0x049B45 << 0x4A4BB9 << 0x0A4B4D << 0xAB25C2 << 0x06A554 << 0x06D449 << 0x6ADA3D << 0x0AB651 << 0x093746 << 0x5497BB; //1981-1990
    lunarCalendarTable << 0x04974F << 0x064B44 << 0x36A537 << 0x0EA54A << 0x86B2BF << 0x05AC53 << 0x0AB647 << 0x5936BC << 0x092E50 << 0x0C9645; //1991-2000
    lunarCalendarTable << 0x4D4AB8 << 0x0D4A4C << 0x0DA541 << 0x25AAB6 << 0x056A49 << 0x7AADBD << 0x025D52 << 0x092D47 << 0x5C95BA << 0x0A954E; //2001-2010
    lunarCalendarTable << 0x0B4A43 << 0x4B5537 << 0x0AD54A << 0x955ABF << 0x04BA53 << 0x0A5B48 << 0x652BBC << 0x052B50 << 0x0A9345 << 0x474AB9; //2011-2020
    lunarCalendarTable << 0x06AA4C << 0x0AD541 << 0x24DAB6 << 0x04B64A << 0x69573D << 0x0A4E51 << 0x0D2646 << 0x5E933A << 0x0D534D << 0x05AA43; //2021-2030
    lunarCalendarTable << 0x36B537 << 0x096D4B << 0xB4AEBF << 0x04AD53 << 0x0A4D48 << 0x6D25BC << 0x0D254F << 0x0D5244 << 0x5DAA38 << 0x0B5A4C; //2031-2040
    lunarCalendarTable << 0x056D41 << 0x24ADB6 << 0x049B4A << 0x7A4BBE << 0x0A4B51 << 0x0AA546 << 0x5B52BA << 0x06D24E << 0x0ADA42 << 0x355B37; //2041-2050
    lunarCalendarTable << 0x09374B << 0x8497C1 << 0x049753 << 0x064B48 << 0x66A53C << 0x0EA54F << 0x06B244 << 0x4AB638 << 0x0AAE4C << 0x092E42; //2051-2060
    lunarCalendarTable << 0x3C9735 << 0x0C9649 << 0x7D4ABD << 0x0D4A51 << 0x0DA545 << 0x55AABA << 0x056A4E << 0x0A6D43 << 0x452EB7 << 0x052D4B; //2061-2070
    lunarCalendarTable << 0x8A95BF << 0x0A9553 << 0x0B4A47 << 0x6B553B << 0x0AD54F << 0x055A45 << 0x4A5D38 << 0x0A5B4C << 0x052B42 << 0x3A93B6; //2071-2080
    lunarCalendarTable << 0x069349 << 0x7729BD << 0x06AA51 << 0x0AD546 << 0x54DABA << 0x04B64E << 0x0A5743 << 0x452738 << 0x0D264A << 0x8E933E; //2081-2090
    lunarCalendarTable << 0x0D5252 << 0x0DAA47 << 0x66B53B << 0x056D4F << 0x04AE45 << 0x4A4EB9 << 0x0A4D4C << 0x0D1541 << 0x2D92B5;             //2091-2099

    //每年春节对应的公历日期
    springFestival << 130 << 217 << 206;                                                                // 1968 1969 1970
    springFestival << 127 << 215 << 203 << 123 << 211 << 131 << 218 << 207 << 128 << 216;               // 1971--1980
    springFestival << 205 << 125 << 213 << 202 << 220 << 209 << 219 << 217 << 206 << 127;               // 1981--1990
    springFestival << 215 << 204 << 123 << 210 << 131 << 219 << 207 << 128 << 216 << 205;               // 1991--2000
    springFestival << 124 << 212 << 201 << 122 << 209 << 129 << 218 << 207 << 126 << 214;               // 2001--2010
    springFestival << 203 << 123 << 210 << 131 << 219 << 208 << 128 << 216 << 205 << 125;               // 2011--2020
    springFestival << 212 << 201 << 122 << 210 << 129 << 217 << 206 << 126 << 213 << 203;               // 2021--2030
    springFestival << 123 << 211 << 131 << 219 << 208 << 128 << 215 << 204 << 124 << 212;               // 2031--2040

    //16--18位表示闰几月 0--12位表示农历每月的数据 高位表示1月 低位表示12月(农历闰月就会多一个月)
    lunarData << 461653 << 1386 << 2413;                                                                // 1968 1969 1970
    lunarData << 330077 << 1197 << 2637 << 268877 << 3365 << 531109 << 2900 << 2922 << 398042 << 2395;  // 1971--1980
    lunarData << 1179 << 267415 << 2635 << 661067 << 1701 << 1748 << 398772 << 2742 << 2391 << 330031;  // 1981--1990
    lunarData << 1175 << 1611 << 200010 << 3749 << 527717 << 1452 << 2742 << 332397 << 2350 << 3222;    // 1991--2000
    lunarData << 268949 << 3402 << 3493 << 133973 << 1386 << 464219 << 605 << 2349 << 334123 << 2709;   // 2001--2010
    lunarData << 2890 << 267946 << 2773 << 592565 << 1210 << 2651 << 395863 << 1323 << 2707 << 265877;  // 2011--2020
    lunarData << 1706 << 2773 << 133557 << 1206 << 398510 << 2638 << 3366 << 335142 << 3411 << 1450;    // 2021--2030
    lunarData << 200042 << 2413 << 723293 << 1197 << 2637 << 399947 << 3365 << 3410 << 334676 << 2906;  // 2031--2040

    //二十四节气表
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 1970
    chineseTwentyFourData << 0x96 << 0xB4 << 0x96 << 0xA6 << 0x97 << 0x97 << 0x78 << 0x79 << 0x79 << 0x69 << 0x78 << 0x77; // 1971
    chineseTwentyFourData << 0x96 << 0xA4 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1972
    chineseTwentyFourData << 0xA5 << 0xB5 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 1973
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 1974
    chineseTwentyFourData << 0x96 << 0xB4 << 0x96 << 0xA6 << 0x97 << 0x97 << 0x78 << 0x79 << 0x78 << 0x69 << 0x78 << 0x77; // 1975
    chineseTwentyFourData << 0x96 << 0xA4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x88 << 0x89 << 0x88 << 0x78 << 0x87 << 0x87; // 1976
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 1977
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x78 << 0x87; // 1978
    chineseTwentyFourData << 0x96 << 0xB4 << 0x96 << 0xA6 << 0x96 << 0x97 << 0x78 << 0x79 << 0x78 << 0x69 << 0x78 << 0x77; // 1979
    chineseTwentyFourData << 0x96 << 0xA4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1980
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x77 << 0x87; // 1981
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 1982
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x78 << 0x79 << 0x78 << 0x69 << 0x78 << 0x77; // 1983
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1984
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA6 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 1985
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 1986
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x79 << 0x78 << 0x69 << 0x78 << 0x87; // 1987
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 1988
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1989
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 1990
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x86 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 1991
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 1992
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1993
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 1994
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x76 << 0x78 << 0x69 << 0x78 << 0x87; // 1995
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 1996
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 1997
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 1998
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 1999
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2000
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2001
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 2002
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 2003
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2004
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2005
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2006
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 2007
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2008
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2009
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2010
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x78 << 0x87; // 2011
    chineseTwentyFourData << 0x96 << 0xB4 << 0xA5 << 0xB5 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2012
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2013
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2014
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2015
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2016
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2017
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA6 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2018
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2019
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x86; // 2020
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2021
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2022
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2023
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2024
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2025
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2026
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 2027
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2028
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2029
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2030
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 2031
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2032
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2033
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x78 << 0x88 << 0x78 << 0x87 << 0x87; // 2034
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2035
    chineseTwentyFourData << 0x95 << 0xB4 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2036
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2037
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2038
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2039
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x69 << 0x78 << 0x87; // 2040
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2041
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2042
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2043
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x97 << 0x88 << 0x78 << 0x78 << 0x79 << 0x78 << 0x87; // 2044
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2045
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2046
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2047
    chineseTwentyFourData << 0x95 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2048
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x86; // 2049
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2050
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2051
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2052
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x86; // 2053
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2054
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2055
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x79 << 0x77 << 0x87; // 2056
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2057
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2058
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2059
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 2060
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2061
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2062
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2063
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0x96 << 0x96 << 0x88 << 0x78 << 0x78 << 0x78 << 0x87 << 0x87; // 2064
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2065
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2066
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2067
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2068
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2069
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2070
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2071
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2072
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x88 << 0x87 << 0x96; // 2073
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA5 << 0xA6 << 0x87 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2074
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2075
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2076
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x88 << 0x87 << 0x96; // 2077
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x88 << 0x87 << 0x78 << 0x87 << 0x86; // 2078
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2079
    chineseTwentyFourData << 0xA5 << 0xB4 << 0x96 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x78 << 0x78 << 0x87 << 0x87; // 2080
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0x97 << 0x87 << 0x87 << 0x88 << 0x86 << 0x96; // 2081
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x86; // 2082
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2083
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA6 << 0xA5 << 0xA6 << 0x96 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2084
    chineseTwentyFourData << 0xB4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0x97 << 0x87 << 0x87 << 0x88 << 0x86 << 0x96; // 2085
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x86; // 2086
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2087
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2088
    chineseTwentyFourData << 0xB4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0x97 << 0x87 << 0x87 << 0x88 << 0x96 << 0x96; // 2089
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2090
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2091
    chineseTwentyFourData << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2092
    chineseTwentyFourData << 0xB4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0x97 << 0x87 << 0x87 << 0x87 << 0x96 << 0x96; // 2093
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2094
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2095
    chineseTwentyFourData << 0xA5 << 0xB3 << 0xA5 << 0xA5 << 0xA6 << 0xA6 << 0x88 << 0x88 << 0x88 << 0x78 << 0x87 << 0x87; // 2096
    chineseTwentyFourData << 0xB4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA5 << 0x97 << 0x97 << 0x87 << 0x87 << 0x96 << 0x96; // 2097
    chineseTwentyFourData << 0xA4 << 0xC3 << 0xA5 << 0xB4 << 0xA5 << 0xA6 << 0x97 << 0x87 << 0x87 << 0x78 << 0x87 << 0x96; // 2098
    chineseTwentyFourData << 0xA5 << 0xC3 << 0xA5 << 0xB5 << 0xA6 << 0xA6 << 0x87 << 0x88 << 0x88 << 0x78 << 0x87 << 0x86; // 2099

    //公历每月前面的天数
    monthAdd << 0 << 31 << 59 << 90 << 120 << 151 << 181 << 212 << 243 << 273 << 304 << 334;

    //农历日期名称
    listDayName << "*" << "初一" << "初二" << "初三" << "初四" << "初五" << "初六" << "初七" << "初八" << "初九" << "初十"
                << "十一" << "十二" << "十三" << "十四" << "十五" << "十六" << "十七" << "十八" << "十九" << "二十"
                << "廿一" << "廿二" << "廿三" << "廿四" << "廿五" << "廿六" << "廿七" << "廿八" << "廿九" << "三十";

    //农历月份名称
    listMonthName << "*" << "正月" << "二月" << "三月" << "四月" << "五月" << "六月" << "七月" << "八月" << "九月" << "十月" << "冬月" << "腊月";

    //二十四节气名称
    listSolarTerm << "小寒" << "大寒" << "立春" << "雨水" << "惊蛰" << "春分" << "清明" << "谷雨"
                  << "立夏" << "小满" << "芒种" << "夏至" << "小暑" << "大暑" << "立秋" << "处暑"
                  << "白露" << "秋分" << "寒露" << "霜降" << "立冬" << "小雪" << "大雪" << "冬至";

    //天干
    listTianGan << "甲" << "乙" << "丙" << "丁" << "戊" << "己" << "庚" << "辛" << "壬" << "癸";

    //地支
    listDiZhi << "子" << "丑" << "寅" << "卯" << "辰" << "巳" << "午" << "未" << "申" << "酉" << "戌" << "亥";

    //属相
    listShuXiang << "鼠" << "牛" << "虎" << "兔" << "龙" << "蛇" << "马" << "羊" << "猴" << "鸡" << "狗" << "猪";
}

//计算是否闰年
bool LunarCalendarInfo::isLoopYear(int year)
{
    return (((0 == (year % 4)) && (0 != (year % 100))) || (0 == (year % 400)));
}

//计算指定年月该月共多少天
int LunarCalendarInfo::getMonthDays(int year, int month)
{
    int countDay = 0;
    int loopDay = isLoopYear(year) ? 1 : 0;

    switch (month) {
        case 1:
            countDay = 31;
            break;
        case 2:
            countDay = 28 + loopDay;
            break;
        case 3:
            countDay = 31;
            break;
        case 4:
            countDay = 30;
            break;
        case 5:
            countDay = 31;
            break;
        case 6:
            countDay = 30;
            break;
        case 7:
            countDay = 31;
            break;
        case 8:
            countDay = 31;
            break;
        case 9:
            countDay = 30;
            break;
        case 10:
            countDay = 31;
            break;
        case 11:
            countDay = 30;
            break;
        case 12:
            countDay = 31;
            break;
        default:
            countDay = 30;
            break;
    }

    return countDay;
}

//计算指定年月对应到该月共多少天
int LunarCalendarInfo::getTotalMonthDays(int year, int month)
{
    int countDay = 0;
    int loopDay = isLoopYear(year) ? 1 : 0;

    switch (month) {
        case 1:
            countDay = 0;
            break;
        case 2:
            countDay = 31;
            break;
        case 3:
            countDay = 59 + loopDay;
            break;
        case 4:
            countDay = 90 + loopDay;
            break;
        case 5:
            countDay = 120 + loopDay;
            break;
        case 6:
            countDay = 151 + loopDay;
            break;
        case 7:
            countDay = 181 + loopDay;
            break;
        case 8:
            countDay = 212 + loopDay;
            break;
        case 9:
            countDay = 243 + loopDay;
            break;
        case 10:
            countDay = 273 + loopDay;
            break;
        case 11:
            countDay = 304 + loopDay;
            break;
        case 12:
            countDay = 334 + loopDay;
            break;
        default:
            countDay = 0;
            break;
    }

    return countDay;
}

//计算指定年月对应星期几
int LunarCalendarInfo::getFirstDayOfWeek(int year, int month)
{
    int week = 0;
    week = (year + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400) % 7;
    week += getTotalMonthDays(year, month);
    week = week % 7;
    return week;
}

//计算国际节日
QString LunarCalendarInfo::getHoliday(int month, int day)
{
    int temp = (month << 8) | day;
    QString strHoliday;

    switch (temp) {
        case 0x0101:
            strHoliday = "元旦";
            break;
        case 0x020E:
            strHoliday = "情人节";
            break;
        case 0x0303:
            strHoliday = "爱耳日";
            break;
        case 0x0305:
            strHoliday = "志愿者服务日";
            break;
        case 0x0308:
            strHoliday = "妇女节";
            break;
        case 0x0309:
            strHoliday = "保护母亲河";
            break;
        case 0x030C:
            strHoliday = "植树节";
            break;
        case 0x030F:
            strHoliday = "消费者权益日";
            break;
        case 0x0401:
            strHoliday = "愚人节";
            break;
        case 0x0501:
            strHoliday = "劳动节";
            break;
        case 0x0504:
            strHoliday = "青年节";
            break;
        case 0x0601:
            strHoliday = "儿童节";
            break;
        case 0x0606:
            strHoliday = "全国爱眼日";
            break;
        case 0x0701:
            strHoliday = "建党节";
            break;
        case 0x0707:
            strHoliday = "抗战纪念日";
            break;
        case 0x0801:
            strHoliday = "建军节";
            break;
        case 0x090A:
            strHoliday = "教师节";
            break;
        case 0x0910:
            strHoliday = "脑健康日";
            break;
        case 0x0914:
            strHoliday = "爱牙日";
            break;
        case 0x0A01:
            strHoliday = "国庆节";
            break;
        case 0x0A0A:
            strHoliday = "高血压日";
            break;
        case 0x0A1C:
            strHoliday = "男性健康日";
            break;
        case 0x0B08:
            strHoliday = "记者节";
            break;
        case 0x0B09:
            strHoliday = "消防宣传日";
            break;
        case 0x0C04:
            strHoliday = "法制宣传日";
            break;
        case 0x0C18:
            strHoliday = "平安夜";
            break;
        case 0x0C19:
            strHoliday = "圣诞节";
            break;
        default:
            break;
    }

    return strHoliday;
}

//计算二十四节气
QString LunarCalendarInfo::getSolarTerms(int year, int month, int day)
{
    QString strSolarTerms;
    int dayTemp = 0;
    //24节气对应表在1970年以后指定?
    int index = (year - 1970) * 12 + month - 1;
    if (index < 0) {
        return "";
    }

    if (day < 15) {
        dayTemp = 15 - day;
        if ((chineseTwentyFourData.at(index) >> 4) == dayTemp) {
            strSolarTerms = listSolarTerm.at(2 * (month - 1));
        }
    } else if (day > 15) {
        dayTemp = day - 15;
        if ((chineseTwentyFourData.at(index) & 0x0f) == dayTemp) {
            strSolarTerms = listSolarTerm.at(2 * (month - 1) + 1);
        }
    }

    return strSolarTerms;
}

//计算农历节日(必须传入农历年份月份)
QString LunarCalendarInfo::getLunarFestival(int month, int day)
{
    int temp = (month << 8) | day;
    QString strFestival;

    switch (temp) {
        case 0x0101:
            strFestival = "春节";
            break;
        case 0x010F:
            strFestival = "元宵节";
            break;
        case 0x0202:
            strFestival = "龙抬头";
            break;
        case 0x0505:
            strFestival = "端午节";
            break;
        case 0x0707:
            strFestival = "七夕节";
            break;
        case 0x080F:
            strFestival = "中秋节";
            break;
        case 0x0909:
            strFestival = "重阳节";
            break;
        case 0x0C08:
            strFestival = "腊八节";
            break;
        case 0x0C1E:
            strFestival = "除夕";
            break;
        default:
            break;
    }

    return strFestival;
}

//计算农历年 天干+地支+生肖
QString LunarCalendarInfo::getLunarYear(int year)
{
    QString strYear;
    if (year > 1924) {
        int temp = year - 1924;
        strYear.append(listTianGan.at(temp % 10));
        strYear.append(listDiZhi.at(temp % 12));
        strYear.append(listShuXiang.at(temp % 12));
    }

    return strYear;
}

void LunarCalendarInfo::getLunarCalendarInfo(int year, int month, int day,
        QString &strHoliday,
        QString &strSolarTerms,
        QString &strLunarFestival,
        QString &strLunarYear,
        QString &strLunarMonth,
        QString &strLunarDay)
{
    //过滤不在范围内的年月日
    if (year < 1901 || year > 2099 || month < 1 || month > 12 || day < 1 || day > 31) {
        return;
    }

    strHoliday = getHoliday(month, day);
    strSolarTerms = getSolarTerms(year, month, day);

#ifndef year_2099
    //现在计算农历:获得当年春节的公历日期(比如：2015年春节日期为(2月19日))
    //以此为分界点,2.19前面的农历是2014年农历(用2014年农历数据来计算)
    //2.19以及之后的日期，农历为2015年农历(用2015年农历数据来计算)
    int temp, tempYear, tempMonth, isEnd, k, n;
    tempMonth = year - 1968;
    int springFestivalMonth = springFestival.at(tempMonth) / 100;
    int springFestivalDaty = springFestival.at(tempMonth) % 100;

    if (month < springFestivalMonth) {
        tempMonth--;
        tempYear = 365 * 1 + day + monthAdd.at(month - 1) - 31 * ((springFestival.at(tempMonth) / 100) - 1) - springFestival.at(tempMonth) % 100 + 1;

        if ((!(year % 4)) && (month > 2)) {
            tempYear = tempYear + 1;
        }

        if ((!((year - 1) % 4))) {
            tempYear = tempYear + 1;
        }
    } else if (month == springFestivalMonth) {
        if (day < springFestivalDaty) {
            tempMonth--;
            tempYear = 365 * 1 + day + monthAdd.at(month - 1) - 31 * ((springFestival.at(tempMonth) / 100) - 1) - springFestival.at(tempMonth) % 100 + 1;

            if ((!(year % 4)) && (month > 2)) {
                tempYear = tempYear + 1;
            }

            if ((!((year - 1) % 4))) {
                tempYear = tempYear + 1;
            }
        } else {
            tempYear = day + monthAdd.at(month - 1) - 31 * ((springFestival.at(tempMonth) / 100) - 1) - springFestival.at(tempMonth) % 100 + 1;

            if ((!(year % 4)) && (month > 2)) {
                tempYear = tempYear + 1;
            }
        }
    } else {
        tempYear = day + monthAdd.at(month - 1) - 31 * ((springFestival.at(tempMonth) / 100) - 1) - springFestival.at(tempMonth) % 100 + 1;

        if ((!(year % 4)) && (month > 2)) {
            tempYear = tempYear + 1;
        }
    }

    //计算农历天干地支月日
    isEnd = 0;
    while (isEnd != 1)  {
        if (lunarData.at(tempMonth) < 4095) {
            k = 11;
        } else {
            k = 12;
        }

        n = k;

        while (n >= 0)   {
            temp = lunarData.at(tempMonth);
            temp = temp >> n;
            temp = temp % 2;

            if (tempYear <= (29 + temp))    {
                isEnd = 1;
                break;
            }

            tempYear = tempYear - 29 - temp;
            n = n - 1;
        }

        if (isEnd) {
            break;
        }

        tempMonth = tempMonth + 1;
    }

    //农历的年月日
    year = 1969 + tempMonth - 1;
    month = k - n + 1;
    day = tempYear;

    if (k == 12)  {
        if (month == (lunarData.at(tempMonth) / 65536) + 1) {
            month = 1 - month;
        } else if (month > (lunarData.at(tempMonth) / 65536) + 1) {
            month = month - 1;
        }
    }

    strLunarYear = getLunarYear(year);

    if (month < 1 && (1 == day)) {
        month = month * -1;
        strLunarMonth = "闰" + listMonthName.at(month);
    } else {
        strLunarMonth = listMonthName.at(month);
    }

    strLunarDay = listDayName.at(day);
    strLunarFestival = getLunarFestival(month, day);
#else
    //记录春节离当年元旦的天数
    int springOffset = 0;
    //记录阳历日离当年元旦的天数
    int newYearOffset = 0;
    //记录大小月的天数 29或30
    int monthCount = 0;

    if (((lunarCalendarTable.at(year - 1901) & 0x0060) >> 5) == 1) {
        springOffset = (lunarCalendarTable.at(year - 1901) & 0x001F) - 1;
    } else {
        springOffset = (lunarCalendarTable.at(year - 1901) & 0x001F) - 1 + 31;
    }

    //如果是不闰年且不是2月份 +1
    newYearOffset = monthAdd.at(month - 1) + day - 1;
    if ((!(year % 4)) && (month > 2)) {
        newYearOffset++;
    }

    //记录从哪个月开始来计算
    int index = 0;
    //用来对闰月的特殊处理
    int flag = 0;

    //判断阳历日在春节前还是春节后,计算农历的月/日
    if (newYearOffset >= springOffset) {
        newYearOffset -= springOffset;
        month = 1;
        index = 1;
        flag = 0;

        if ((lunarCalendarTable.at(year - 1901) & (0x80000 >> (index - 1))) == 0) {
            monthCount = 29;
        } else {
            monthCount = 30;
        }

        while (newYearOffset >= monthCount) {
            newYearOffset -= monthCount;
            index++;
            if (month == ((lunarCalendarTable.at(year - 1901) & 0xF00000) >> 20)) {
                flag = ~flag;
                if (flag == 0) {
                    month++;
                }
            } else {
                month++;
            }

            if ((lunarCalendarTable.at(year - 1901) & (0x80000 >> (index - 1))) == 0) {
                monthCount = 29;
            } else {
                monthCount = 30;
            }
        }

        day = newYearOffset + 1;
    } else {
        springOffset -= newYearOffset;
        year--;
        month = 12;
        flag = 0;

        if (((lunarCalendarTable.at(year - 1901) & 0xF00000) >> 20) == 0) {
            index = 12;
        } else {
            index = 13;
        }

        if ((lunarCalendarTable.at(year - 1901) & (0x80000 >> (index - 1))) == 0) {
            monthCount = 29;
        } else {
            monthCount = 30;
        }

        while (springOffset > monthCount) {
            springOffset -= monthCount;
            index--;

            if (flag == 0) {
                month--;
            }

            if (month == ((lunarCalendarTable.at(year - 1901) & 0xF00000) >> 20)) {
                flag = ~flag;
            }

            if ((lunarCalendarTable.at(year - 1901) & (0x80000 >> (index - 1))) == 0) {
                monthCount = 29;
            } else {
                monthCount = 30;
            }
        }

        day = monthCount - springOffset + 1;
    }

    //计算农历的索引配置
    int temp = 0;
    temp |= day;
    temp |= (month << 6);

    //转换农历的年月
    month = (temp & 0x3C0) >> 6;
    day = temp & 0x3F;

    strLunarYear = getLunarYear(year);

    if ((month == ((lunarCalendarTable.at(year - 1901) & 0xF00000) >> 20)) && (1 == day)) {
        strLunarMonth = "闰" + listMonthName.at(month);
    } else {
        strLunarMonth = listMonthName.at(month);
    }

    strLunarDay = listDayName.at(day);
    strLunarFestival = getLunarFestival(month, day);
#endif
}

QString LunarCalendarInfo::getLunarInfo(int year, int month, int day, bool yearInfo, bool monthInfo, bool dayInfo)
{
    QString strHoliday, strSolarTerms, strLunarFestival, strLunarYear, strLunarMonth, strLunarDay;

    LunarCalendarInfo::Instance()->getLunarCalendarInfo(year, month, day,
            strHoliday, strSolarTerms, strLunarFestival,
            strLunarYear, strLunarMonth, strLunarDay);

    //农历节日优先,其次农历节气,然后公历节日,最后才是农历月份名称
    if (!strLunarFestival.isEmpty()) {
        strLunarDay = strLunarFestival;
    } else if (!strSolarTerms.isEmpty()) {
        strLunarDay = strSolarTerms;
    } else if (!strHoliday.isEmpty()) {
        strLunarDay = strHoliday;
    }

    QString info = QString("%1%2%3")
                   .arg(yearInfo ? strLunarYear + "年" : "")
                   .arg(monthInfo ? strLunarMonth : "")
                   .arg(dayInfo ? strLunarDay : "");

    return info;
}

QString LunarCalendarInfo::getLunarYearMonthDay(int year, int month, int day)
{
    return getLunarInfo(year, month, day, true, true, true);
}

QString LunarCalendarInfo::getLunarMonthDay(int year, int month, int day)
{
    return getLunarInfo(year, month, day, false, true, true);
}

QString LunarCalendarInfo::getLunarDay(int year, int month, int day)
{
    return getLunarInfo(year, month, day, false, false, true);
}
