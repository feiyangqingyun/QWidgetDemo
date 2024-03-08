#include "customstyle.h"
#include "qapplication.h"
#include "qpalette.h"

void CustomStyle::initStyle(int fontSize, int radioButtonSize, int checkBoxSize, int sliderHeight)
{
    if (fontSize <= 0) {
        return;
    }

    QStringList list;
    //全局字体
    list << QString("*{font-size:%1px;}").arg(fontSize);
    //单选框
    list << QString("QRadioButton::indicator{width:%1px;height:%1px;}").arg(radioButtonSize);
    //复选框
    list << QString("QCheckBox::indicator,QGroupBox::indicator,QTreeWidget::indicator,QListWidget::indicator{width:%1px;height:%1px;}").arg(checkBoxSize);

    //滑块颜色
#if 0
    QString normalColor = "#e3e3e3";
    QString grooveColor = "#0078d7";
    QString handleColor = "#FFFFFF";
    QString borderColor = "#9B9B9B";
#else
    QPalette palette;
    for (int i = 0; i < 21; ++i) {
        //qDebug() << i << palette.color((QPalette::ColorRole)i).name();
    }

    QString normalColor = palette.color(QPalette::Midlight).name();
    QString grooveColor = palette.color(QPalette::Highlight).name();
    QString handleColor = palette.color(QPalette::Light).name();
    QString borderColor = palette.color(QPalette::Shadow).name();
#endif
    int sliderRadius = sliderHeight / 2;
    int handleWidth = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleWidth / 2 + 1;
    int handleOffset = handleRadius / 2;

    //横向滑块
    list << QString("QSlider::horizontal{min-height:%1px;}").arg(sliderHeight * 2);
    list << QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
         .arg(normalColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
         .arg(normalColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
         .arg(grooveColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::handle:horizontal{border:1px solid %5;width:%2px;margin-top:-%3px;margin-bottom:-%3px;border-radius:%4px;"
                    "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 #FFFFFF,stop:0.8 %1);}")
         .arg(handleColor).arg(handleWidth).arg(handleOffset).arg(handleRadius).arg(borderColor);

    //垂直滑块
    list << QString("QSlider::vertical{min-width:%1px;}").arg(sliderHeight * 2);
    list << QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
         .arg(normalColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
         .arg(grooveColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
         .arg(normalColor).arg(sliderHeight).arg(sliderRadius);
    list << QString("QSlider::handle:vertical{border:1px solid %5;height:%2px;margin-left:-%3px;margin-right:-%3px;border-radius:%4px;"
                    "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 #FFFFFF,stop:0.8 %1);}")
         .arg(handleColor).arg(handleWidth).arg(handleOffset).arg(handleRadius).arg(borderColor);

    qApp->setStyleSheet(list.join(""));
}
