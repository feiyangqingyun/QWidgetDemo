#ifndef APPDATA_H
#define APPDATA_H

#include "head.h"

class AppData
{
public:
    static QString TitleFlag;       //标题标识
    static int RowHeight;           //行高
    static int RightWidth;          //右侧宽度
    static int FormWidth;           //窗体宽度
    static int FormHeight;          //窗体高度

    static void checkRatio();       //校验分辨率    
};

#endif // APPDATA_H
