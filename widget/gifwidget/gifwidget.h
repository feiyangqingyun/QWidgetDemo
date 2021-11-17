#ifndef GIFWIDGET_H
#define GIFWIDGET_H

/**
 * GIF录屏控件 作者:feiyangqingyun(QQ:517216493) 2019-04-03
 * 1. 可设置要录制屏幕的宽高，支持右下角直接拉动改变。
 * 2. 可设置变宽的宽度。
 * 3. 可设置录屏控件的背景颜色。
 * 4. 可设置录制的帧数。
 * 5. 录制区域可自由拖动选择。
 */

#include <QDialog>
#include "gif.h"

class QLineEdit;
class QLabel;

#ifdef quc
class Q_DECL_EXPORT GifWidget : public QDialog
#else
class GifWidget : public QDialog
#endif

{
    Q_OBJECT

    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)

public:
    static GifWidget *Instance();
    explicit GifWidget(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:
    static QScopedPointer<GifWidget> self;
    QWidget *widgetTop;         //标题栏
    QWidget *widgetMain;        //中间部分
    QWidget *widgetBottom;      //底部栏
    QLineEdit *txtFps;          //帧率输入框
    QLineEdit *txtWidth;        //宽度输入框
    QLineEdit *txtHeight;       //高度输入框
    QPushButton *btnStart;      //开始按钮
    QLabel *labStatus;          //显示状态信息

    int fps;                    //帧数 100为1s
    int borderWidth;            //边框宽度
    QColor bgColor;             //背景颜色

    int count;                  //帧数计数
    QString fileName;           //保存文件名称
    QRect rectGif;              //截屏区域
    QTimer *timer;              //截屏定时器

    Gif gif;                    //gif类对象
    Gif::GifWriter *gifWriter;  //gif写入对象

public:
    int getBorderWidth()        const;
    QColor getBgColor()         const;

private slots:
    void initControl();
    void initForm();
    void saveImage();
    void record();
    void closeAll();
    void resizeForm();

public Q_SLOTS:
    void setBorderWidth(int borderWidth);
    void setBgColor(const QColor &bgColor);
};

#endif // GIFWIDGET_H
