#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

/**
 * 通用视频播放控件 作者:feiyangqingyun(QQ:517216493) 2018-05-01
 * 1. 可设置边框大小。
 * 2. 可设置边框颜色。
 * 3. 可设置两路OSD标签。
 * 4. 可设置是否绘制OSD标签。
 * 5. 可设置标签文本或图片。
 * 6. 可设置OSD位置 左上角、左下角、右上角、右下角。
 * 7. 可设置OSD风格 文本、日期、时间、日期时间、图片。
 * 8. 自定义半透明悬浮窗体，一排按钮。
 * 9. 悬浮按钮可自定义设置，包括背景颜色+按下颜色。
 * 10. 发送信号通知单击了哪个悬浮按钮。
 * 11. 能够识别拖进来的文件，通知url。
 * 12. 提供open close pause等接口。
 */

#include <QWidget>
#include <QDateTime>

class QTimer;

#ifdef quc
class Q_DECL_EXPORT VideoWindow : public QWidget
#else
class VideoWindow : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(OSDFormat)
    Q_ENUMS(OSDPosition)

    Q_PROPERTY(bool copyImage READ getCopyImage WRITE setCopyImage)
    Q_PROPERTY(bool checkLive READ getCheckLive WRITE setCheckLive)
    Q_PROPERTY(bool drawImage READ getDrawImage WRITE setDrawImage)
    Q_PROPERTY(bool fillImage READ getFillImage WRITE setFillImage)

    Q_PROPERTY(bool flowEnable READ getFlowEnable WRITE setFlowEnable)
    Q_PROPERTY(QColor flowBgColor READ getFlowBgColor WRITE setFlowBgColor)
    Q_PROPERTY(QColor flowPressColor READ getFlowPressColor WRITE setFlowPressColor)

    Q_PROPERTY(int timeout READ getTimeout WRITE setTimeout)
    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(QColor focusColor READ getFocusColor WRITE setFocusColor)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
    Q_PROPERTY(QString bgText READ getBgText WRITE setBgText)
    Q_PROPERTY(QImage bgImage READ getBgImage WRITE setBgImage)

    Q_PROPERTY(bool osd1Visible READ getOSD1Visible WRITE setOSD1Visible)
    Q_PROPERTY(int osd1FontSize READ getOSD1FontSize WRITE setOSD1FontSize)
    Q_PROPERTY(QString osd1Text READ getOSD1Text WRITE setOSD1Text)
    Q_PROPERTY(QColor osd1Color READ getOSD1Color WRITE setOSD1Color)
    Q_PROPERTY(QImage osd1Image READ getOSD1Image WRITE setOSD1Image)
    Q_PROPERTY(OSDFormat osd1Format READ getOSD1Format WRITE setOSD1Format)
    Q_PROPERTY(OSDPosition osd1Position READ getOSD1Position WRITE setOSD1Position)

    Q_PROPERTY(bool osd2Visible READ getOSD2Visible WRITE setOSD2Visible)
    Q_PROPERTY(int osd2FontSize READ getOSD2FontSize WRITE setOSD2FontSize)
    Q_PROPERTY(QString osd2Text READ getOSD2Text WRITE setOSD2Text)
    Q_PROPERTY(QColor osd2Color READ getOSD2Color WRITE setOSD2Color)
    Q_PROPERTY(QImage osd2Image READ getOSD2Image WRITE setOSD2Image)
    Q_PROPERTY(OSDFormat osd2Format READ getOSD2Format WRITE setOSD2Format)
    Q_PROPERTY(OSDPosition osd2Position READ getOSD2Position WRITE setOSD2Position)

public:
    //标签格式
    enum OSDFormat {
        OSDFormat_Text = 0,             //文本
        OSDFormat_Date = 1,             //日期
        OSDFormat_Time = 2,             //时间
        OSDFormat_DateTime = 3,         //日期时间
        OSDFormat_Image = 4             //图片
    };

    //标签位置
    enum OSDPosition {
        OSDPosition_Left_Top = 0,       //左上角
        OSDPosition_Left_Bottom = 1,    //左下角
        OSDPosition_Right_Top = 2,      //右上角
        OSDPosition_Right_Bottom = 3    //右下角
    };

    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();

protected:
    void resizeEvent(QResizeEvent *);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    void enterEvent(QEnterEvent *);
#else
    void enterEvent(QEvent *);
#endif
    void leaveEvent(QEvent *);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void paintEvent(QPaintEvent *);
    void drawBorder(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawImg(QPainter *painter, QImage img);
    void drawOSD(QPainter *painter,
                 bool osdVisible,
                 int osdFontSize,
                 const QString &osdText,
                 const QColor &osdColor,
                 const QImage &osdImage,
                 const OSDFormat &osdFormat,
                 const OSDPosition &osdPosition);

private:
    QTimer *timerCheck;             //定时器检查设备是否在线
    QImage image;                   //要显示的图片
    QWidget *flowPanel;             //悬浮条面板

    bool copyImage;                 //是否拷贝图片
    bool checkLive;                 //检测是否活着
    bool drawImage;                 //是否绘制图片
    bool fillImage;                 //自动拉伸填充

    bool flowEnable;                //是否显示悬浮条
    QColor flowBgColor;             //悬浮条背景颜色
    QColor flowPressColor;          //悬浮条按下颜色

    int timeout;                    //超时时间
    int borderWidth;                //边框宽度
    QColor borderColor;             //边框颜色
    QColor focusColor;              //有焦点边框颜色
    QColor bgColor;                 //背景颜色
    QString bgText;                 //默认无图像显示文字
    QImage bgImage;                 //默认无图像背景图片

    bool osd1Visible;               //显示标签1
    int osd1FontSize;               //标签1字号
    QString osd1Text;               //标签1文本
    QColor osd1Color;               //标签1颜色
    QImage osd1Image;               //标签1图片
    OSDFormat osd1Format;           //标签1文本格式
    OSDPosition osd1Position;       //标签1位置

    bool osd2Visible;               //显示标签2
    int osd2FontSize;               //标签2字号
    QString osd2Text;               //标签2文本
    QColor osd2Color;               //标签2颜色
    QImage osd2Image;               //标签2图片
    OSDFormat osd2Format;           //标签2文本格式
    OSDPosition osd2Position;       //标签2位置

    int faceBorder;                 //人脸框粗细
    QColor faceColor;               //人脸框颜色
    QList<QRect> faceRects;         //人脸框集合

private:
    //初始化解码线程
    void initThread();
    //初始化悬浮条
    void initFlowPanel();
    //初始化悬浮条样式
    void initFlowStyle();

public:
    QImage getImage()               const;
    QPixmap getPixmap()             const;
    QString getUrl()                const;
    QDateTime getLastTime()         const;

    bool getCallback()              const;
    bool getIsPlaying()             const;
    bool getIsRtsp()                const;
    bool getIsUsbCamera()           const;

    bool getCopyImage()             const;
    bool getCheckLive()             const;
    bool getDrawImage()             const;
    bool getFillImage()             const;

    bool getFlowEnable()            const;
    QColor getFlowBgColor()         const;
    QColor getFlowPressColor()      const;

    int getTimeout()                const;
    int getBorderWidth()            const;
    QColor getBorderColor()         const;
    QColor getFocusColor()          const;
    QColor getBgColor()             const;
    QString getBgText()             const;
    QImage getBgImage()             const;

    bool getOSD1Visible()           const;
    int getOSD1FontSize()           const;
    QString getOSD1Text()           const;
    QColor getOSD1Color()           const;
    QImage getOSD1Image()           const;
    OSDFormat getOSD1Format()       const;
    OSDPosition getOSD1Position()   const;

    bool getOSD2Visible()           const;
    int getOSD2FontSize()           const;
    QString getOSD2Text()           const;
    QColor getOSD2Color()           const;
    QImage getOSD2Image()           const;
    OSDFormat getOSD2Format()       const;
    OSDPosition getOSD2Position()   const;

    int getFaceBorder()             const;
    QColor getFaceColor()           const;
    QList<QRect> getFaceRects()     const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

private slots:
    //接收图像并绘制
    void updateImage(const QImage &image);
    //校验设备
    void checkVideo();
    //处理按钮单击
    void btnClicked();

signals:
    //播放成功
    void receivePlayStart();
    //播放失败
    void receivePlayError();
    //播放结束
    void receivePlayFinsh();

    //总时长
    void fileLengthReceive(qint64 length);
    //当前播放时长
    void filePositionReceive(qint64 position);

    //收到图片信号
    void receiveImage(const QImage &image);

    //接收到拖曳文件
    void fileDrag(const QString &url);

    //工具栏单击
    void btnClicked(const QString &objName);

public slots:
    //获取长度
    uint getLength();
    //获取当前播放位置
    uint getPosition();
    //设置播放位置
    void setPosition(int position);

    //获取静音状态
    bool getMuted();
    //设置静音
    void setMuted(bool muted);

    //获取音量
    int getVolume();
    //设置音量
    void setVolume(int volume);

    //设置显示间隔
    void setInterval(int interval);
    //设置休眠时间
    void setSleepTime(int sleepTime);
    //设置检测连接超时
    void setCheckTime(int checkTime);
    //设置是否检测连接
    void setCheckConn(bool checkConn);

    //设置视频流地址
    void setUrl(const QString &url);
    //设置是否采用回调
    void setCallback(bool callback);
    //设置硬件解码器名称
    void setHardware(const QString &hardware);
    //设置通信协议
    void setTransport(const QString &transport);

    //设置是否保存文件
    void setSaveFile(bool saveFile);
    //设置保存间隔
    void setSaveInterval(int saveInterval);
    //设置定时保存文件唯一标识符
    void setFileFlag(const QString &fileFlag);
    //设置保存文件夹
    void setSavePath(const QString &savePath);
    //设置保存文件名称
    void setFileName(const QString &fileName);

    //设置是否拷贝图片
    void setCopyImage(bool copyImage);
    //设置是否检测活着
    void setCheckLive(bool checkLive);
    //设置是否实时绘制图片
    void setDrawImage(bool drawImage);
    //设置是否拉伸填充
    void setFillImage(bool fillImage);

    //设置是否启用悬浮条
    void setFlowEnable(bool flowEnable);
    //设置悬浮条背景颜色
    void setFlowBgColor(const QColor &flowBgColor);
    //设置悬浮条按下颜色
    void setFlowPressColor(const QColor &flowPressColor);

    //设置超时时间
    void setTimeout(int timeout);
    //设置边框宽度
    void setBorderWidth(int borderWidth);
    //设置边框颜色
    void setBorderColor(const QColor &borderColor);
    //设置有焦点边框颜色
    void setFocusColor(const QColor &focusColor);
    //设置背景颜色
    void setBgColor(const QColor &bgColor);
    //设置无图像文字
    void setBgText(const QString &bgText);
    //设置无图像背景图
    void setBgImage(const QImage &bgImage);

    //设置标签1是否可见
    void setOSD1Visible(bool osdVisible);
    //设置标签1文字字号
    void setOSD1FontSize(int osdFontSize);
    //设置标签1文本
    void setOSD1Text(const QString &osdText);
    //设置标签1文字颜色
    void setOSD1Color(const QColor &osdColor);
    //设置标签1图片
    void setOSD1Image(const QImage &osdImage);
    //设置标签1格式
    void setOSD1Format(const OSDFormat &osdFormat);
    //设置标签1位置
    void setOSD1Position(const OSDPosition &osdPosition);

    //设置标签2是否可见
    void setOSD2Visible(bool osdVisible);
    //设置标签2文字字号
    void setOSD2FontSize(int osdFontSize);
    //设置标签2文本
    void setOSD2Text(const QString &osdText);
    //设置标签2文字颜色
    void setOSD2Color(const QColor &osdColor);
    //设置标签2图片
    void setOSD2Image(const QImage &osdImage);
    //设置标签2格式
    void setOSD2Format(const OSDFormat &osdFormat);
    //设置标签2位置
    void setOSD2Position(const OSDPosition &osdPosition);

    //设置值自动进行枚举转换
    void setOSD1Format(quint8 osdFormat);
    void setOSD2Format(quint8 osdFormat);
    void setOSD1Position(quint8 osdPosition);
    void setOSD2Position(quint8 osdPosition);

    //设置人脸框粗细
    void setFaceBorder(int faceBorder);
    //设置人脸框颜色
    void setFaceColor(const QColor &faceColor);
    //设置人脸框区域集合
    void setFaceRects(const QList<QRect> &faceRects);

    //打开设备
    void open();
    //暂停播放
    void pause();
    //继续播放
    void next();
    //关闭设备
    void close();
    //重新加载
    void restart(const QString &url, int delayOpen = 500);
    //清空图片
    void clear();
    //截图快照
    void snap(const QString &fileName);
};

#endif // VIDEOWINDOW_H
