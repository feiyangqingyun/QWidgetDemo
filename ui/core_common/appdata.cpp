#include "appdata.h"
#include "quihelper.h"

QString AppData::TitleFlag = "(QQ: 517216493 WX: feiyangqingyun)";
int AppData::RowHeight = 25;
int AppData::RightWidth = 180;
int AppData::FormWidth = 1200;
int AppData::FormHeight = 750;

void AppData::checkRatio()
{
    //根据分辨率设定宽高
    int width = QUIHelper::deskWidth();
    if (width >= 1440) {
        RowHeight = RowHeight < 25 ? 25 : RowHeight;
        RightWidth = RightWidth < 220 ? 220 : RightWidth;
        FormWidth = FormWidth < 1200 ? 1200 : FormWidth;
        FormHeight = FormHeight < 800 ? 800 : FormHeight;
    }
}
