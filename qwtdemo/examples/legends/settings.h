#ifndef _SETTINGS_
#define _SETTINGS_

#include <qstring.h>

class Settings
{
public:
    Settings()
    {
        legend.isEnabled = false;
        legend.position = 0;

        legendItem.isEnabled = false;
        legendItem.numColumns = 0;
        legendItem.alignment = 0;
        legendItem.backgroundMode = 0;
        legendItem.size = 12;

        curve.numCurves = 0;
        curve.title = "Curve";
    }
    
    struct
    {
        bool isEnabled;
        int position;
    } legend;

    struct
    {
        bool isEnabled;
        int numColumns;
        int alignment;
        int backgroundMode;
        int size;
        
    } legendItem;
    
    struct
    {
        int numCurves;
        QString title;
    } curve;
};

#endif
