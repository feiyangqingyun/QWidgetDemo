#ifndef IMAGESWITCH_H
#define IMAGESWITCH_H

/**
 * 图片开关控件 作者:feiyangqingyun(QQ:517216493) 2016-11-25
 * 1. 自带三种开关按钮样式。
 * 2. 可自定义开关图片。
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT ImageSwitch : public QWidget
#else
class ImageSwitch : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(ButtonStyle)

    Q_PROPERTY(bool isChecked READ getChecked WRITE setChecked)
    Q_PROPERTY(ButtonStyle buttonStyle READ getButtonStyle WRITE setButtonStyle)

public:
    enum ButtonStyle {
        ButtonStyle_1 = 0,  //开关样式1
        ButtonStyle_2 = 1,  //开关样式2
        ButtonStyle_3 = 2   //开关样式3
    };

    explicit ImageSwitch(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);

private:
    bool isChecked;
    ButtonStyle buttonStyle;

    QString imgOffFile;
    QString imgOnFile;
    QString imgFile;

public:
    bool getChecked()               const;
    ButtonStyle getButtonStyle()    const;
    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    void setChecked(bool isChecked);
    void setButtonStyle(const ImageSwitch::ButtonStyle &buttonStyle);
};

#endif // IMAGESWITCH_H
