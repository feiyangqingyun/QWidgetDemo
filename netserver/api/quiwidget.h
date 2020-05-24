#ifndef QUIWIDGET_H
#define QUIWIDGET_H

#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME            qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#define QDATE           qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME           qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME        qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME     qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS   qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))

#ifdef Q_OS_WIN
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

#ifdef __arm__
#define TitleMinSize 40
#else
#define TitleMinSize 30
#endif

/**
 * QUI无边框窗体控件 作者:feiyangqingyun(QQ:517216493)
 * 1:内置 N >= 12 套精美样式,可直接切换,也可自定义样式路径
 * 2:可设置部件(左上角图标/最小化按钮/最大化按钮/关闭按钮)的图标或者图片及是否可见
 * 3:可集成设计师插件,直接拖曳使用,所见即所得
 * 4:如果需要窗体可拖动大小,设置 setSizeGripEnabled(true);
 * 5:可设置全局样式 setStyle
 * 6:可弹出消息框,可选阻塞模式和不阻塞,默认不阻塞 showMessageBoxInfo
 * 7:可弹出错误框,可选阻塞模式和不阻塞,默认不阻塞 showMessageBoxError
 * 8:可弹出询问框 showMessageBoxError
 * 9:可弹出输入框 showInputBox
 * 10:消息框支持设置倒计时关闭
 * 11:集成图形字体设置方法及根据指定文字获取图片
 * 12:集成设置窗体居中显示/设置翻译文件/设置编码/设置延时/设置系统时间等静态方法
 * 13:集成获取应用程序文件名/文件路径 等方法
 */

#include "head.h"
#include "app.h"

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT QUIWidget : public QDialog
#else
class QUIWidget : public QDialog
#endif

{
    Q_OBJECT
    Q_ENUMS(Style)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(Qt::Alignment alignment READ getAlignment WRITE setAlignment)

public:
    //将部分对象作为枚举值暴露给外部
    enum Widget {
        Lab_Ico = 0,                //左上角图标
        BtnMenu = 1,                //下拉菜单按钮
        BtnMenu_Min = 2,            //最小化按钮
        BtnMenu_Max = 3,            //最大化按钮
        BtnMenu_Normal = 4,         //还原按钮
        BtnMenu_Close = 5           //关闭按钮
    };

    //样式枚举
    enum Style {
        Style_Silvery = 0,          //银色样式
        Style_Blue = 1,             //蓝色样式
        Style_LightBlue = 2,        //淡蓝色样式
        Style_DarkBlue = 3,         //深蓝色样式
        Style_Gray = 4,             //灰色样式
        Style_LightGray = 5,        //浅灰色样式
        Style_DarkGray = 6,         //深灰色样式
        Style_Black = 7,            //黑色样式
        Style_LightBlack = 8,       //浅黑色样式
        Style_DarkBlack = 12,        //深黑色样式
        Style_PSBlack = 10,         //PS黑色样式
        Style_FlatBlack = 11,       //黑色扁平样式
        Style_FlatWhite = 12        //白色扁平样式
    };

public:
    explicit QUIWidget(QWidget *parent = 0);
    ~QUIWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    QVBoxLayout *verticalLayout1;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout4;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QToolButton *btnMenu;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Max;
    QPushButton *btnMenu_Close;
    QWidget *widget;
    QVBoxLayout *verticalLayout3;

private:
    QString title;                  //标题
    Qt::Alignment alignment;        //标题文本对齐
    bool minHide;                   //最小化隐藏
    QWidget *mainWidget;            //主窗体对象

public:
    QLabel *getLabIco()             const;
    QLabel *getLabTitle()           const;
    QToolButton *getBtnMenu()       const;
    QPushButton *getBtnMenuMin()    const;
    QPushButton *getBtnMenuMax()    const;
    QPushButton *getBtnMenuMClose() const;

    Style getStyle()                const;
    QString getTitle()              const;
    Qt::Alignment getAlignment()    const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void changeStyle();             //更换样式

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    //设置部件图标
    void setIcon(QUIWidget::Widget widget, QChar str, quint32 size = 12);
    void setIconMain(QChar str, quint32 size = 12);
    //设置部件图片
    void setPixmap(QUIWidget::Widget widget, const QString &file, const QSize &size = QSize(16, 16));
    //设置部件是否可见
    void setVisible(QUIWidget::Widget widget, bool visible = true);
    //设置只有关闭按钮
    void setOnlyCloseBtn();

    //设置标题栏高度
    void setTitleHeight(int height);
    //设置按钮统一宽度
    void setBtnWidth(int width);

    //设置标题及文本样式
    void setTitle(const QString &title);
    void setAlignment(Qt::Alignment alignment);

    //设置最小化隐藏
    void setMinHide(bool minHide);

    //设置主窗体
    void setMainWidget(QWidget *mainWidget);

Q_SIGNALS:
    void changeStyle(const QString &qssFile);
    void closing();
};

//弹出信息框类
class QUIMessageBox : public QDialog
{
    Q_OBJECT

public:
    static QUIMessageBox *Instance();
    explicit QUIMessageBox(QWidget *parent = 0);
    ~QUIMessageBox();

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static QUIMessageBox *self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout3;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout4;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout4;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIcoMain;
    QSpacerItem *horizontalSpacer1;
    QLabel *labInfo;
    QHBoxLayout *horizontalLayout2;
    QSpacerItem *horizontalSpacer2;
    QPushButton *btnOk;
    QPushButton *btnCancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(QChar str, quint32 size = 12);
    void setMessage(const QString &msg, int type, int closeSec = 0);
};

//弹出输入框类
class QUIInputBox : public QDialog
{
    Q_OBJECT

public:
    static QUIInputBox *Instance();
    explicit QUIInputBox(QWidget *parent = 0);
    ~QUIInputBox();

protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static QUIInputBox *self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout2;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout3;
    QLabel *labInfo;
    QLineEdit *txtValue;
    QComboBox *cboxValue;
    QHBoxLayout *lay;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

private:
    int closeSec;                   //总显示时间
    int currentSec;                 //当前已显示时间
    QString value;                  //当前值

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体
    void checkSec();                //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    QString getValue()const;

public Q_SLOTS:
    void setIconMain(QChar str, quint32 size = 12);
    void setParameter(const QString &title, int type = 0, int closeSec = 0,
                      QString defaultValue = QString(), bool pwd = false);

};

//弹出日期选择对话框
class QUIDateSelect : public QDialog
{
    Q_OBJECT

public:
    static QUIDateSelect *Instance();
    explicit QUIDateSelect(QWidget *parent = 0);
    ~QUIDateSelect();

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static QUIDateSelect *self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout1;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *labStart;
    QPushButton *btnOk;
    QLabel *labEnd;
    QPushButton *btnClose;
    QDateTimeEdit *dateStart;
    QDateTimeEdit *dateEnd;

private:
    QString startDateTime;          //开始时间
    QString endDateTime;            //结束时间
    QString format;                 //日期时间格式

private slots:
    void initControl();             //初始化控件
    void initForm();                //初始化窗体

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    //获取当前选择的开始时间和结束时间
    QString getStartDateTime()  const;
    QString getEndDateTime()    const;

public Q_SLOTS:
    void setIconMain(QChar str, quint32 size = 12);
    void setFormat(const QString &format);

};

//全局静态方法类
class QUIHelper : public QObject
{
    Q_OBJECT
public:
    //桌面宽度高度
    static int deskWidth();
    static int deskHeight();

    //程序本身文件名称
    static QString appName();
    //程序当前所在路径
    static QString appPath();

    //初始化随机数种子
    static void initRand();

    //新建目录
    static void newDir(const QString &dirName);

    //写入消息到额外的的消息日志文件
    static void writeInfo(const QString &info, const QString &filePath = "log");
    static void writeError(const QString &info, const QString &filePath = "log");

    //设置全局样式
    static void setStyle(QUIWidget::Style style);
    static void setStyle(const QString &qssFile, QString &paletteColor, QString &textColor);
    static void setStyle(const QString &qssFile, QString &textColor,
                         QString &panelColor, QString &borderColor,
                         QString &normalColorStart, QString &normalColorEnd,
                         QString &darkColorStart, QString &darkColorEnd,
                         QString &highColor);

    //根据QSS样式获取对应颜色值
    static void getQssColor(const QString &qss, QString &textColor,
                            QString &panelColor, QString &borderColor,
                            QString &normalColorStart, QString &normalColorEnd,
                            QString &darkColorStart, QString &darkColorEnd,
                            QString &highColor);

    //九宫格图片 horzSplit-宫格1/3/7/9宽度  vertSplit-宫格1/3/7/9高度  dstWidth-目标图片宽度  dstHeight-目标图片高度
    static QPixmap ninePatch(const QString &picName, int horzSplit, int vertSplit, int dstWidth, int dstHeight);
    static QPixmap ninePatch(const QPixmap &pix, int horzSplit, int vertSplit, int dstWidth, int dstHeight);

    //设置标签颜色
    static void setLabStyle(QLabel *lab, quint8 type);

    //设置窗体居中显示
    static void setFormInCenter(QWidget *frm);
    //设置翻译文件
    static void setTranslator(const QString &qmFile = ":/image/qt_zh_CN.qm");
    //设置编码
    static void setCode();
    //设置延时
    static void sleep(int sec);
    //设置系统时间
    static void setSystemDateTime(const QString &year, const QString &month, const QString &day,
                                  const QString &hour, const QString &min, const QString &sec);
    //设置开机自启动
    static void runWithSystem(const QString &strName, const QString &strPath, bool autoRun = true);

    //判断是否是IP地址
    static bool isIP(const QString &ip);

    //判断是否是MAC地址
    static bool isMac(const QString &mac);

    //判断是否是合法的电话号码
    static bool isTel(const QString &tel);

    //判断是否是合法的邮箱地址
    static bool isEmail(const QString &email);


    //16进制字符串转10进制
    static int strHexToDecimal(const QString &strHex);

    //10进制字符串转10进制
    static int strDecimalToDecimal(const QString &strDecimal);

    //2进制字符串转10进制
    static int strBinToDecimal(const QString &strBin);

    //16进制字符串转2进制字符串
    static QString strHexToStrBin(const QString &strHex);

    //10进制转2进制字符串一个字节
    static QString decimalToStrBin1(int decimal);

    //10进制转2进制字符串两个字节
    static QString decimalToStrBin2(int decimal);

    //10进制转16进制字符串,补零.
    static QString decimalToStrHex(int decimal);


    //int转字节数组
    static QByteArray intToByte(int i);
    static QByteArray intToByteRec(int i);

    //字节数组转int
    static int byteToInt(const QByteArray &data);
    static int byteToIntRec(const QByteArray &data);

    //ushort转字节数组
    static QByteArray ushortToByte(ushort i);
    static QByteArray ushortToByteRec(ushort i);

    //字节数组转ushort
    static int byteToUShort(const QByteArray &data);
    static int byteToUShortRec(const QByteArray &data);

    //异或加密算法
    static QString getXorEncryptDecrypt(const QString &str, char key);

    //异或校验
    static uchar getOrCode(const QByteArray &data);

    //计算校验码
    static uchar getCheckCode(const QByteArray &data);

    //字符串补全
    static QString getValue(quint8 value);

    //CRC校验
    static quint16 getRevCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getModbus16(quint8 *data, int len);
    static QByteArray getCRCCode(const QByteArray &data);


    //字节数组转Ascii字符串
    static QString byteArrayToAsciiStr(const QByteArray &data);

    //16进制字符串转字节数组
    static QByteArray hexStrToByteArray(const QString &str);
    static char convertHexChar(char ch);

    //Ascii字符串转字节数组
    static QByteArray asciiStrToByteArray(const QString &str);

    //字节数组转16进制字符串
    static QString byteArrayToHexStr(const QByteArray &data);

    //获取保存的文件
    static QString getSaveName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());

    //获取选择的文件
    static QString getFileName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());

    //获取选择的文件集合
    static QStringList getFileNames(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());

    //获取选择的目录
    static QString getFolderName();

    //获取文件名,含拓展名
    static QString getFileNameWithExtension(const QString &strFilePath);

    //获取选择文件夹中的文件
    static QStringList getFolderFileNames(const QStringList &filter);

    //文件夹是否存在
    static bool folderIsExist(const QString &strFolder);

    //文件是否存在
    static bool fileIsExist(const QString &strFile);

    //复制文件
    static bool copyFile(const QString &sourceFile, const QString &targetFile);

    //删除文件夹下所有文件
    static void deleteDirectory(const QString &path);

    //判断IP地址及端口是否在线
    static bool ipLive(const QString &ip, int port, int timeout = 1000);

    //获取网页所有源代码
    static QString getHtml(const QString &url);

    //获取本机公网IP地址
    static QString getNetIP(const QString &webCode);

    //获取本机IP
    static QString getLocalIP();

    //Url地址转为IP地址
    static QString urlToIP(const QString &url);

    //判断是否通外网
    static bool isWebOk();


    //弹出消息框
    static void showMessageBoxInfo(const QString &info, int closeSec = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &info, int closeSec = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &info);

    //弹出输入框
    static QString showInputBox(const QString &title, int type = 0, int closeSec = 0,
                                QString defaultValue = QString(), bool pwd = false);

    //弹出日期选择框
    static void showDateSelect(QString &dateStart, QString &dateEnd, const QString &format = "yyyy-MM-dd");

};

//图形字体处理类
class IconHelper : public QObject
{
    Q_OBJECT

public:
    static IconHelper *Instance();
    explicit IconHelper(QObject *parent = 0);

    void setIcon(QLabel *lab, QChar c, quint32 size = 12);
    void setIcon(QAbstractButton *btn, QChar c, quint32 size = 12);
    QPixmap getPixmap(const QString &color, QChar c, quint32 size = 12,
                      quint32 pixWidth = 10, quint32 pixHeight = 10);

    //根据按钮获取该按钮对应的图标
    QPixmap getPixmap(QToolButton *btn, bool normal);

    //指定导航面板样式,不带图标
    static void setStyle(QWidget *widget, const QString &type = "left", int borderWidth = 3,
                         const QString &borderColor = "#029FEA",
                         const QString &normalBgColor = "#292F38",
                         const QString &darkBgColor = "#1D2025",
                         const QString &normalTextColor = "#54626F",
                         const QString &darkTextColor = "#FDFDFD");

    //移除导航面板样式,防止重复
    void removeStyle(QList<QToolButton *> btns);

    //指定QWidget导航面板样式,带图标和效果切换
    void setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> pixChar,
                  quint32 iconSize = 12, quint32 iconWidth = 10, quint32 iconHeight = 10,
                  const QString &type = "left", int borderWidth = 3,
                  const QString &borderColor = "#029FEA",
                  const QString &normalBgColor = "#292F38",
                  const QString &darkBgColor = "#1D2025",
                  const QString &normalTextColor = "#54626F",
                  const QString &darkTextColor = "#FDFDFD");

    //指定QFrame导航按钮样式,带图标
    void setStyle(QFrame *frame, QList<QToolButton *> btns, QList<int> pixChar,
                  quint32 iconSize = 12, quint32 iconWidth = 10, quint32 iconHeight = 10,
                  const QString &normalBgColor = "#2FC5A2",
                  const QString &darkBgColor = "#3EA7E9",
                  const QString &normalTextColor = "#EEEEEE",
                  const QString &darkTextColor = "#FFFFFF");

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static IconHelper *self;    //对象自身
    QFont iconFont;             //图形字体
    QList<QToolButton *> btns;  //按钮队列
    QList<QPixmap> pixNormal;   //正常图片队列
    QList<QPixmap> pixDark;     //加深图片队列
};

//全局变量控制
class QUIConfig
{
public:
    //全局图标
    static QChar IconMain;          //标题栏左上角图标
    static QChar IconMenu;          //下拉菜单图标
    static QChar IconMin;           //最小化图标
    static QChar IconMax;           //最大化图标
    static QChar IconNormal;        //还原图标
    static QChar IconClose;         //关闭图标

    static QString FontName;        //全局字体名称
    static int FontSize;            //全局字体大小

    //样式表颜色值
    static QString TextColor;       //文字颜色
    static QString PanelColor;      //面板颜色
    static QString BorderColor;     //边框颜色
    static QString NormalColorStart;//正常状态开始颜色
    static QString NormalColorEnd;  //正常状态结束颜色
    static QString DarkColorStart;  //加深状态开始颜色
    static QString DarkColorEnd;    //加深状态结束颜色
    static QString HighColor;       //高亮颜色
};

#endif // QUIWIDGET_H
