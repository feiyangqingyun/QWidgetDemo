#ifndef QUIWIDGET_H
#define QUIWIDGET_H

#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME            qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#define QDATE           qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME           qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME        qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME     qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS   qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))

//定义换行符
#ifdef Q_OS_WIN
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

//定义无边框标题栏高度+对话框最小宽高
#ifdef __arm__
#define TitleMinSize 40
#define DialogMinWidth 350
#define DialogMinHeight 180
#else
#define TitleMinSize 30
#define DialogMinWidth 280
#define DialogMinHeight 150
#endif

/**
 * QUI无边框窗体控件 作者:feiyangqingyun(QQ:517216493)
 * 1:内置 N >= 17 套精美样式,可直接切换,也可自定义样式路径
 * 2:可设置部件(左上角图标/最小化按钮/最大化按钮/关闭按钮)的图标或者图片及是否可见
 * 3:可集成设计师插件,直接拖曳使用,所见即所得
 * 4:如果需要窗体可拖动大小,设置 setSizeGripEnabled(true);
 * 5:可设置全局样式 setStyle
 * 6:可弹出消息框,可选阻塞模式和不阻塞,默认不阻塞 showMessageBoxInfo
 * 7:可弹出错误框,可选阻塞模式和不阻塞,默认不阻塞 showMessageBoxError
 * 8:可弹出询问框 showMessageBoxError
 * 9:可弹出右下角信息框 showTipBox
 * 10:可弹出输入框 showInputBox
 * 11:可弹出时间范围选择框 showDateSelect
 * 12:消息框支持设置倒计时关闭
 * 13:集成图形字体设置方法及根据指定文字获取图片
 * 14:集成设置窗体居中显示/设置翻译文件/设置编码/设置延时/设置系统时间等静态方法
 * 15:集成获取应用程序文件名/文件路径 等方法
 */

#include "head.h"

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
    Q_PROPERTY(bool minHide READ getMinHide WRITE setMinHide)
    Q_PROPERTY(bool exitAll READ getExitAll WRITE setExitAll)

public:
    //将部分对象作为枚举值暴露给外部
    enum Widget {
        Lab_Ico = 0,            //左上角图标
        BtnMenu = 1,            //下拉菜单按钮
        BtnMenu_Min = 2,        //最小化按钮
        BtnMenu_Max = 3,        //最大化按钮
        BtnMenu_Normal = 4,     //还原按钮
        BtnMenu_Close = 5       //关闭按钮
    };

    //样式枚举
    enum Style {
        Style_Silvery = 0,      //银色样式
        Style_Blue = 1,         //蓝色样式
        Style_LightBlue = 2,    //淡蓝色样式
        Style_DarkBlue = 3,     //深蓝色样式
        Style_Gray = 4,         //灰色样式
        Style_LightGray = 5,    //浅灰色样式
        Style_DarkGray = 6,     //深灰色样式
        Style_Black = 7,        //黑色样式
        Style_LightBlack = 8,   //浅黑色样式
        Style_DarkBlack = 9,    //深黑色样式
        Style_PSBlack = 10,     //PS黑色样式
        Style_FlatBlack = 11,   //黑色扁平样式
        Style_FlatWhite = 12,   //白色扁平样式
        Style_FlatBlue = 13,    //蓝色扁平样式
        Style_Purple = 14,      //紫色样式
        Style_BlackBlue = 15,   //黑蓝色样式
        Style_BlackVideo = 16   //视频监控黑色样式
    };

public:
    explicit QUIWidget(QWidget *parent = 0);
    ~QUIWidget();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

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
    QString title;          //标题
    Qt::Alignment alignment;//标题文本对齐
    bool minHide;           //最小化隐藏
    bool exitAll;           //退出整个程序
    QWidget *mainWidget;    //主窗体对象

public:
    QLabel *getLabIco()             const;
    QLabel *getLabTitle()           const;
    QToolButton *getBtnMenu()       const;
    QPushButton *getBtnMenuMin()    const;
    QPushButton *getBtnMenuMax()    const;
    QPushButton *getBtnMenuMClose() const;

    QString getTitle()              const;
    Qt::Alignment getAlignment()    const;
    bool getMinHide()               const;
    bool getExitAll()               const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void changeStyle(); //更换样式

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    //设置部件图标
    void setIcon(QUIWidget::Widget widget, const QChar &str, quint32 size = 12);
    void setIconMain(const QChar &str, quint32 size = 12);
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

    //设置退出时候直接退出整个应用程序
    void setExitAll(bool exitAll);

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
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIMessageBox> self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout3;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labCountDown;
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
    int closeSec;       //总显示时间
    int currentSec;     //当前已显示时间

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void checkSec();    //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setIconMsg(const QString &png, const QChar &str);
    void setMessage(const QString &msg, int type, int closeSec = 0);
};

//右下角弹出框类
class QUITipBox : public QDialog
{
    Q_OBJECT

public:
    static QUITipBox *Instance();
    explicit QUITipBox(QWidget *parent = 0);
    ~QUITipBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUITipBox> self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout2;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labCountDown;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QLabel *labInfo;

    QPropertyAnimation *animation;
    bool fullScreen;

private:
    int closeSec;       //总显示时间
    int currentSec;     //当前已显示时间

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void checkSec();    //校验倒计时

private slots:
    void on_btnMenu_Close_clicked();

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setTip(const QString &title, const QString &tip, bool fullScreen = false, bool center = true, int closeSec = 0);
    void hide();
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
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIInputBox> self;

    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labCountDown;
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
    int closeSec;       //总显示时间
    int currentSec;     //当前已显示时间
    QString value;      //当前值

private slots:
    void initControl(); //初始化控件
    void initForm();    //初始化窗体
    void checkSec();    //校验倒计时

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    QString getValue()const;

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setParameter(const QString &title, int type = 0, int closeSec = 0,
                      QString placeholderText = QString(), bool pwd = false,
                      const QString &defaultValue = QString());

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
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIDateSelect> self;

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
    QString startDateTime;  //开始时间
    QString endDateTime;    //结束时间
    QString format;         //日期时间格式

private slots:
    void initControl();     //初始化控件
    void initForm();        //初始化窗体

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    //获取当前选择的开始时间和结束时间
    QString getStartDateTime()  const;
    QString getEndDateTime()    const;

public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setFormat(const QString &format);

};

//图形字体处理类
class IconHelper : public QObject
{
    Q_OBJECT

public:
    static IconHelper *Instance();
    explicit IconHelper(QObject *parent = 0);

    //获取图形字体
    QFont getIconFont();

    //设置图形字体到标签
    void setIcon(QLabel *lab, const QChar &str, quint32 size = 12);
    //设置图形字体到按钮
    void setIcon(QAbstractButton *btn, const QChar &str, quint32 size = 12);

    //获取指定图形字体,可以指定文字大小,图片宽高,文字对齐
    QPixmap getPixmap(const QColor &color, const QChar &str, quint32 size = 12,
                      quint32 pixWidth = 15, quint32 pixHeight = 15,
                      int flags = Qt::AlignCenter);

    //根据按钮获取该按钮对应的图标
    QPixmap getPixmap(QToolButton *btn, bool normal);
    QPixmap getPixmap(QToolButton *btn, int type);

    //指定QFrame导航按钮样式,带图标
    void setStyle(QFrame *frame, QList<QToolButton *> btns, QList<int> pixChar,
                  quint32 iconSize = 12, quint32 iconWidth = 15, quint32 iconHeight = 15,
                  const QString &normalBgColor = "#2FC5A2",
                  const QString &darkBgColor = "#3EA7E9",
                  const QString &normalTextColor = "#EEEEEE",
                  const QString &darkTextColor = "#FFFFFF");

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
                  quint32 iconSize = 12, quint32 iconWidth = 15, quint32 iconHeight = 15,
                  const QString &type = "left", int borderWidth = 3,
                  const QString &borderColor = "#029FEA",
                  const QString &normalBgColor = "#292F38",
                  const QString &darkBgColor = "#1D2025",
                  const QString &normalTextColor = "#54626F",
                  const QString &darkTextColor = "#FDFDFD");

    struct StyleColor {
        quint32 iconSize;
        quint32 iconWidth;
        quint32 iconHeight;
        quint32 borderWidth;
        QString type;
        QString borderColor;
        QString normalBgColor;
        QString normalTextColor;
        QString hoverBgColor;
        QString hoverTextColor;
        QString pressedBgColor;
        QString pressedTextColor;
        QString checkedBgColor;
        QString checkedTextColor;

        StyleColor() {
            iconSize = 12;
            iconWidth = 15;
            iconHeight = 15;
            borderWidth = 3;
            type = "left";
            borderColor = "#029FEA";
            normalBgColor = "#292F38";
            normalTextColor = "#54626F";
            hoverBgColor = "#40444D";
            hoverTextColor = "#FDFDFD";
            pressedBgColor = "#404244";
            pressedTextColor = "#FDFDFD";
            checkedBgColor = "#44494F";
            checkedTextColor = "#FDFDFD";
        }
    };

    //指定QWidget导航面板样式,带图标和效果切换+悬停颜色+按下颜色+选中颜色
    void setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> pixChar, const StyleColor &styleColor);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<IconHelper> self;

    QFont iconFont;             //图形字体
    QList<QToolButton *> btns;  //按钮队列
    QList<QPixmap> pixNormal;   //正常图片队列
    QList<QPixmap> pixDark;     //加深图片队列
    QList<QPixmap> pixHover;    //悬停图片队列
    QList<QPixmap> pixPressed;  //按下图片队列
    QList<QPixmap> pixChecked;  //选中图片队列
};

//托盘图标类
class TrayIcon : public QObject
{
    Q_OBJECT
public:
    static TrayIcon *Instance();
    explicit TrayIcon(QObject *parent = 0);

private:
    static QScopedPointer<TrayIcon> self;

    QWidget *mainWidget;            //对应所属主窗体
    QSystemTrayIcon *trayIcon;      //托盘对象
    QMenu *menu;                    //右键菜单
    bool exitDirect;                //是否直接退出

private slots:
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);

public Q_SLOTS:
    //设置是否直接退出,如果不是直接退出则发送信号给主界面
    void setExitDirect(bool exitDirect);

    //设置所属主窗体
    void setMainWidget(QWidget *mainWidget);

    //显示消息
    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 5000);

    //设置图标
    void setIcon(const QString &strIcon);
    //设置提示信息
    void setToolTip(const QString &tip);
    //设置是否可见
    void setVisible(bool visible);
    //退出所有
    void closeAll();

Q_SIGNALS:
    void trayIconExit();
};


//全局静态方法类
class QUIHelper : public QObject
{
    Q_OBJECT
public:
    //桌面宽度高度
    static int deskWidth();
    static int deskHeight();

    //程序文件名称+当前所在路径
    static QString appName();
    static QString appPath();

    //初始化随机数种子
    static void initRand();
    //获取uuid
    static QString getUuid();

    //初始化数据库
    static void initDb(const QString &dbName);
    //初始化文件,不存在则拷贝
    static void initFile(const QString &sourceName, const QString &targetName);

    //检查ini配置文件
    static bool checkIniFile(const QString &iniFile);

    //设置图标到按钮
    static void setIconBtn(QAbstractButton *btn, const QString &png, const QChar &str);

    //新建目录
    static void newDir(const QString &dirName);

    //写入消息到额外的的消息日志文件
    static void writeInfo(const QString &info, bool needWrite = false, const QString &filePath = "log");
    static void writeError(const QString &info, bool needWrite = false, const QString &filePath = "log");

    //设置无边框窗体
    static void setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle, QLabel *labIco, QPushButton *btnClose, bool tool = true);

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
    static void setLabStyle(QLabel *lab, quint8 type, const QString &bgColor = "", const QString &textColor = "");

    //设置窗体居中显示
    static void setFormInCenter(QWidget *frm);
    //设置翻译文件
    static void setTranslator(const QString &qmFile = ":/image/qt_zh_CN.qm");
    //设置编码
    static void setCode();
    //设置字体
    static void setFont(const QString &ttfFile = ":/image/DroidSansFallback.ttf",
                        const QString &fontName = "Microsoft Yahei", int fontSize = 12);
    //设置延时
    static void sleep(int msec);
    //设置系统时间
    static void setSystemDateTime(const QString &year, const QString &month, const QString &day,
                                  const QString &hour, const QString &min, const QString &sec);
    //设置开机自启动
    static void runWithSystem(const QString &strName, const QString &strPath, bool autoRun = true);

    //从字符串获取IP地址
    static QString getIP(const QString &url);
    //判断是否是IP地址
    static bool isIP(const QString &ip);
    //判断是否是MAC地址
    static bool isMac(const QString &mac);
    //判断是否是合法的电话号码
    static bool isTel(const QString &tel);
    //判断是否是合法的邮箱地址
    static bool isEmail(const QString &email);

    //IP地址字符串与整型转换
    static QString ipv4IntToString(quint32 ip);
    static quint32 ipv4StringToInt(const QString &ip);

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
    static quint32 byteToUInt(const QByteArray &data);
    static quint32 byteToUIntRec(const QByteArray &data);

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

    //CRC校验
    static quint16 getRevCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getCrc_16(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getModbus16(quint8 *data, int len);
    static QByteArray getCRCCode(const QByteArray &data);

    //字节数组与Ascii字符串互转
    static void initAsciiStr();
    static QString byteArrayToAsciiStr(const QByteArray &data);
    static QByteArray asciiStrToByteArray(const QString &data);

    //16进制字符串与字节数组互转
    static char hexStrToChar(char data);
    static QByteArray hexStrToByteArray(const QString &data);
    static QString byteArrayToHexStr(const QByteArray &data);

    //获取保存的文件
    static QString getSaveName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());
    //获取选择的文件
    static QString getFileName(const QString &filter, QString defaultDir = QCoreApplication::applicationDirPath());
    //非阻塞保存文件对话框
    static QString saveFileName(const QString &filter, const QString &defaultDir = "", const QString &fileName = "");
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
    static QString getNetIP(const QString &html);
    //获取本机IP
    static QString getLocalIP();
    //获取本机IP地址集合
    static QStringList getLocalIPs();
    //Url地址转为IP地址
    static QString urlToIP(const QString &url);

    //字符串补全
    static QString getValue(quint8 value);
    //判断是否通外网
    static bool isWebOk();

    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);

    //弹出框
    static int showMessageBox(const QString &info, int type = 0, int closeSec = 0, bool exec = false);
    //弹出消息框
    static void showMessageBoxInfo(const QString &info, int closeSec = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &info, int closeSec = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &info);

    //弹出+隐藏右下角信息框
    static void showTipBox(const QString &title, const QString &tip, bool fullScreen = false,
                           bool center = true, int closeSec = 0);
    static void hideTipBox();

    //弹出输入框
    static QString showInputBox(const QString &title, int type = 0, int closeSec = 0,
                                const QString &placeholderText = QString(), bool pwd = false,
                                const QString &defaultValue = QString());
    //弹出日期选择框
    static void showDateSelect(QString &dateStart, QString &dateEnd, const QString &format = "yyyy-MM-dd");


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
    static QString setProgressBarQss(QProgressBar *bar,
                                     int barHeight = 8,                             //进度条高度
                                     int barRadius = 5,                             //进度条半径
                                     int fontSize = 9,                              //文字字号
                                     const QString &normalColor = "#E8EDF2",        //正常颜色
                                     const QString &chunkColor = "#E74C3C");        //进度颜色
    //设置滑块条样式
    static QString setSliderQss(QSlider *slider,                                    //滑动条对象
                                int sliderHeight = 8,                               //滑动条高度
                                const QString &normalColor = "#E8EDF2",             //正常颜色
                                const QString &grooveColor = "#1ABC9C",             //滑块颜色
                                const QString &handleBorderColor = "#1ABC9C",       //指示器边框颜色
                                const QString &handleColor = "#FFFFFF",             //指示器颜色
                                const QString &textColor = "#000000");              //文字颜色
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
