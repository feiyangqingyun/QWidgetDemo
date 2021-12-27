#ifndef FLATUI_H
#define FLATUI_H

/**
 * FlatUI辅助类 作者:feiyangqingyun(QQ:517216493) 2016-12-16
 * 1. 按钮样式设置。
 * 2. 文本框样式设置。
 * 3. 进度条样式。
 * 4. 滑块条样式。
 * 5. 单选框样式。
 * 6. 滚动条样式。
 * 7. 可自由设置对象的高度宽度大小等。
 * 8. 自带默认参数值。
 */

#include <QObject>

class QPushButton;
class QLineEdit;
class QProgressBar;
class QSlider;
class QRadioButton;
class QCheckBox;
class QScrollBar;

#ifdef quc
class Q_DECL_EXPORT FlatUI
#else
class FlatUI
#endif

{
public:
    //设置按钮样式
    static QString setPushButtonQss(QPushButton *btn,                               //按钮对象
                                    int radius = 5,                                 //圆角半径
                                    int padding = 8,                                //间距
                                    const QString &normalColor = "#34495E",         //正常颜色
                                    const QString &normalTextColor = "#FFFFFF",     //文字颜色
                                    const QString &hoverColor = "#4E6D8C",          //悬停颜色
                                    const QString &hoverTextColor = "#F0F0F0",      //悬停文字颜色
                                    const QString &pressedColor = "#2D3E50",        //按下颜色
                                    const QString &pressedTextColor = "#B8C6D1");   //按下文字颜色

    //设置文本框样式
    static QString setLineEditQss(QLineEdit *txt,                                   //文本框对象
                                  int radius = 3,                                   //圆角半径
                                  int borderWidth = 2,                              //边框大小
                                  const QString &normalColor = "#DCE4EC",           //正常颜色
                                  const QString &focusColor = "#34495E");           //选中颜色

    //设置进度条样式
    static QString setProgressQss(QProgressBar *bar,
                                  int barHeight = 8,                                //进度条高度
                                  int barRadius = 5,                                //进度条半径
                                  int fontSize = 12,                                //文字字号
                                  const QString &normalColor = "#E8EDF2",           //正常颜色
                                  const QString &chunkColor = "#E74C3C");           //进度颜色

    //设置滑块条样式
    static QString setSliderQss(QSlider *slider,                                    //滑动条对象
                                int sliderHeight = 8,                               //滑动条高度
                                const QString &normalColor = "#E8EDF2",             //正常颜色
                                const QString &grooveColor = "#1ABC9C",             //滑块颜色
                                const QString &handleBorderColor = "#1ABC9C",       //指示器边框颜色
                                const QString &handleColor = "#FFFFFF");            //指示器颜色

    //设置单选框样式
    static QString setRadioButtonQss(QRadioButton *rbtn,                            //单选框对象
                                     int indicatorRadius = 8,                       //指示器圆角角度
                                     const QString &normalColor = "#D7DBDE",        //正常颜色
                                     const QString &checkColor = "#34495E");        //选中颜色

    //设置滚动条样式
    static QString setScrollBarQss(QWidget *scroll,                                 //滚动条对象
                                   int radius = 6,                                  //圆角角度
                                   int min = 120,                                   //指示器最小长度
                                   int max = 12,                                    //滚动条最大长度
                                   const QString &bgColor = "#606060",              //背景色
                                   const QString &handleNormalColor = "#34495E",    //指示器正常颜色
                                   const QString &handleHoverColor = "#1ABC9C",     //指示器悬停颜色
                                   const QString &handlePressedColor = "#E74C3C");  //指示器按下颜色
};

#endif // FLATUI_H
