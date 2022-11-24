#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H

#include <QObject>

class CustomStyle
{
public:
    //全局样式比如放大选择器
    static void initStyle(int fontSize = 15, int radioButtonSize = 18, int checkBoxSize = 16, int sliderHeight = 13);
};

#endif // CUSTOMSTYLE_H
