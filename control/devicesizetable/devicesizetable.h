#ifndef DEVICESIZETABLE_H
#define DEVICESIZETABLE_H

/**
 * 本地存储空间大小控件 作者:feiyangqingyun(QQ:517216493) 2016-11-30
 * 1. 可自动加载本地存储设备的总容量/已用容量。
 * 2. 进度条显示已用容量。
 * 3. 支持所有操作系统。
 * 4. 增加U盘或者SD卡到达信号。
 */

#include <QTableWidget>

class QProcess;

#ifdef quc
class Q_DECL_EXPORT DeviceSizeTable : public QTableWidget
#else
class DeviceSizeTable : public QTableWidget
#endif

{
    Q_OBJECT

    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
    Q_PROPERTY(QColor chunkColor1 READ getChunkColor1 WRITE setChunkColor1)
    Q_PROPERTY(QColor chunkColor2 READ getChunkColor2 WRITE setChunkColor2)
    Q_PROPERTY(QColor chunkColor3 READ getChunkColor3 WRITE setChunkColor3)

    Q_PROPERTY(QColor textColor1 READ getTextColor1 WRITE setTextColor1)
    Q_PROPERTY(QColor textColor2 READ getTextColor2 WRITE setTextColor2)
    Q_PROPERTY(QColor textColor3 READ getTextColor3 WRITE setTextColor3)

public:
    explicit DeviceSizeTable(QWidget *parent = 0);

private:
    QProcess *process;      //执行命令进程

    QColor bgColor;         //背景颜色
    QColor chunkColor1;     //进度颜色1
    QColor chunkColor2;     //进度颜色2
    QColor chunkColor3;     //进度颜色3

    QColor textColor1;      //文字颜色1
    QColor textColor2;      //文字颜色2
    QColor textColor3;      //文字颜色3

private slots:
    void readData();
    void checkSize(const QString &result, const QString &name);
    void insertSize(const QString &name, const QString &use, const QString &free, const QString &all, int percent);

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置背景颜色
    QColor getBgColor() const;
    void setBgColor(const QColor &bgColor);

    //获取和设置进度颜色1
    QColor getChunkColor1() const;
    void setChunkColor1(const QColor &chunkColor1);

    //获取和设置进度颜色2
    QColor getChunkColor2() const;
    void setChunkColor2(const QColor &chunkColor2);

    //获取和设置进度颜色3
    QColor getChunkColor3() const;
    void setChunkColor3(const QColor &chunkColor3);

    //获取和设置文字颜色1
    QColor getTextColor1() const;
    void setTextColor1(const QColor &textColor1);

    //获取和设置文字颜色2
    QColor getTextColor2() const;
    void setTextColor2(const QColor &textColor2);

    //获取和设置文字颜色3
    QColor getTextColor3() const;
    void setTextColor3(const QColor &textColor3);

public Q_SLOTS:
    //载入容量
    void load();

Q_SIGNALS:
    void sdcardReceive(const QString &sdcardName);
    void udiskReceive(const QString &udiskName);
};

#endif // DEVICESIZETABLE_H
