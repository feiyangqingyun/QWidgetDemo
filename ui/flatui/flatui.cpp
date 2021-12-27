#pragma execution_character_set("utf-8")

#include "flatui.h"
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qprogressbar.h"
#include "qslider.h"
#include "qradiobutton.h"
#include "qcheckbox.h"
#include "qscrollbar.h"
#include "qdebug.h"

QString FlatUI::setPushButtonQss(QPushButton *btn, int radius, int padding,
                                 const QString &normalColor,
                                 const QString &normalTextColor,
                                 const QString &hoverColor,
                                 const QString &hoverTextColor,
                                 const QString &pressedColor,
                                 const QString &pressedTextColor)
{
    QStringList list;
    list.append(QString("QPushButton{border-style:none;padding:%1px;border-radius:%2px;color:%3;background:%4;}")
                .arg(padding).arg(radius).arg(normalTextColor).arg(normalColor));
    list.append(QString("QPushButton:hover{color:%1;background:%2;}")
                .arg(hoverTextColor).arg(hoverColor));
    list.append(QString("QPushButton:pressed{color:%1;background:%2;}")
                .arg(pressedTextColor).arg(pressedColor));

    QString qss = list.join("");
    btn->setStyleSheet(qss);
    return qss;
}

QString FlatUI::setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                               const QString &normalColor,
                               const QString &focusColor)
{
    QStringList list;
    list.append(QString("QLineEdit{border-style:none;padding:3px;border-radius:%1px;border:%2px solid %3;}")
                .arg(radius).arg(borderWidth).arg(normalColor));
    list.append(QString("QLineEdit:focus{border:%1px solid %2;}")
                .arg(borderWidth).arg(focusColor));

    QString qss = list.join("");
    txt->setStyleSheet(qss);
    return qss;
}

QString FlatUI::setProgressQss(QProgressBar *bar, int barHeight,
                               int barRadius, int fontSize,
                               const QString &normalColor,
                               const QString &chunkColor)
{

    QStringList list;
    list.append(QString("QProgressBar{font:%1px;background:%2;max-height:%3px;border-radius:%4px;text-align:center;border:1px solid %2;}")
                .arg(fontSize).arg(normalColor).arg(barHeight).arg(barRadius));
    list.append(QString("QProgressBar:chunk{border-radius:%2px;background-color:%1;}")
                .arg(chunkColor).arg(barRadius));

    QString qss = list.join("");
    bar->setStyleSheet(qss);
    return qss;
}

QString FlatUI::setSliderQss(QSlider *slider, int sliderHeight,
                             const QString &normalColor,
                             const QString &grooveColor,
                             const QString &handleBorderColor,
                             const QString &handleColor)
{
    int sliderRadius = sliderHeight / 2;
    int handleWidth = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleWidth / 2;
    int handleOffset = handleRadius / 2;

    QStringList list;
    list.append(QString("QSlider::horizontal{min-height:%1px;}").arg(sliderHeight * 2));
    list.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:horizontal{width:%3px;margin-top:-%4px;margin-bottom:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    //偏移一个像素
    handleWidth = handleWidth + 1;
    list.append(QString("QSlider::vertical{min-width:%1px;}").arg(sliderHeight * 2));
    list.append(QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:vertical{height:%3px;margin-left:-%4px;margin-right:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    QString qss = list.join("");
    slider->setStyleSheet(qss);
    return qss;
}

QString FlatUI::setRadioButtonQss(QRadioButton *rbtn, int indicatorRadius,
                                  const QString &normalColor,
                                  const QString &checkColor)
{
    int indicatorWidth = indicatorRadius * 2;

    QStringList list;
    list.append(QString("QRadioButton::indicator{border-radius:%1px;width:%2px;height:%2px;}")
                .arg(indicatorRadius).arg(indicatorWidth));
    list.append(QString("QRadioButton::indicator::unchecked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                        "stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(normalColor));
    list.append(QString("QRadioButton::indicator::checked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                        "stop:0 %1,stop:0.3 %1,stop:0.4 #FFFFFF,stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(checkColor));

    QString qss = list.join("");
    rbtn->setStyleSheet(qss);
    return qss;
}

QString FlatUI::setScrollBarQss(QWidget *scroll, int radius, int min, int max,
                                const QString &bgColor,
                                const QString &handleNormalColor,
                                const QString &handleHoverColor,
                                const QString &handlePressedColor)
{
    //滚动条离背景间隔
    int padding = 0;

    QStringList list;

    //handle:指示器,滚动条拉动部分 add-page:滚动条拉动时增加的部分 sub-page:滚动条拉动时减少的部分 add-line:递增按钮 sub-line:递减按钮

    //横向滚动条部分
    list.append(QString("QScrollBar:horizontal{background:%1;padding:%2px;border-radius:%3px;min-height:%4px;max-height:%4px;}")
                .arg(bgColor).arg(padding).arg(radius).arg(max));
    list.append(QString("QScrollBar::handle:horizontal{background:%1;min-width:%2px;border-radius:%3px;}")
                .arg(handleNormalColor).arg(min).arg(radius));
    list.append(QString("QScrollBar::handle:horizontal:hover{background:%1;}")
                .arg(handleHoverColor));
    list.append(QString("QScrollBar::handle:horizontal:pressed{background:%1;}")
                .arg(handlePressedColor));
    list.append(QString("QScrollBar::add-page:horizontal{background:none;}"));
    list.append(QString("QScrollBar::sub-page:horizontal{background:none;}"));
    list.append(QString("QScrollBar::add-line:horizontal{background:none;}"));
    list.append(QString("QScrollBar::sub-line:horizontal{background:none;}"));

    //纵向滚动条部分
    list.append(QString("QScrollBar:vertical{background:%1;padding:%2px;border-radius:%3px;min-width:%4px;max-width:%4px;}")
                .arg(bgColor).arg(padding).arg(radius).arg(max));
    list.append(QString("QScrollBar::handle:vertical{background:%1;min-height:%2px;border-radius:%3px;}")
                .arg(handleNormalColor).arg(min).arg(radius));
    list.append(QString("QScrollBar::handle:vertical:hover{background:%1;}")
                .arg(handleHoverColor));
    list.append(QString("QScrollBar::handle:vertical:pressed{background:%1;}")
                .arg(handlePressedColor));
    list.append(QString("QScrollBar::add-page:vertical{background:none;}"));
    list.append(QString("QScrollBar::sub-page:vertical{background:none;}"));
    list.append(QString("QScrollBar::add-line:vertical{background:none;}"));
    list.append(QString("QScrollBar::sub-line:vertical{background:none;}"));

    QString qss = list.join("");
    scroll->setStyleSheet(qss);
    return qss;
}
