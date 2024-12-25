#ifndef QTHELPER_H
#define QTHELPER_H

#include "head.h"

class QtHelper
{
public:
    //获取所有屏幕区域/当前鼠标所在屏幕索引/区域尺寸/缩放系数
    static bool useRatio;
    static QList<QRect> getScreenRects(bool available = true, bool full = false);
    static int getScreenIndex();
    static QRect getScreenRect(bool available = true, bool full = false);
    static qreal getScreenRatio(int index = -1, bool devicePixel = false);

    //矫正当前鼠标所在屏幕居中尺寸
    static QRect checkCenterRect(QRect &rect, bool available = true);

    //获取桌面宽度高度+居中显示
    static int deskWidth();
    static int deskHeight();
    static QSize deskSize();

    //居中显示窗体
    //定义标志位指定是以桌面为参照还是主程序界面为参照
    static QWidget *centerBaseForm;
    static void setFormInCenter(QWidget *form);
    static void showForm(QWidget *form);

    //程序文件名称和当前所在路径
    static QString appName();
    static QString appPath();

    //程序最前面获取应用程序路径和名称
    static void getCurrentInfo(char *argv[], QString &path, QString &name);
    //程序最前面读取配置文件节点的值
    static QString getIniValue(const QString &fileName, const QString &key);
    static QString getIniValue(char *argv[], const QString &key, const QString &dir = QString(), const QString &file = QString());

    //获取本地网卡IP集合
    static QStringList getLocalIPs();
    //添加网卡集合并根据默认值设置当前项
    static void initLocalIPs(QComboBox *cbox, const QString &defaultIP, bool local127 = true);

    //获取内置颜色集合
    static QList<QColor> colors;
    static QList<QColor> getColorList();
    static QStringList getColorNames();
    //随机获取颜色集合中的颜色
    static QColor getRandColor();

    //初始化随机数种子
    static void initRand();
    //获取随机小数
    static float getRandFloat(float min, float max);
    //获取随机数,指定最小值和最大值
    static double getRandValue(int min, int max, bool contansMin = false, bool contansMax = false);
    //获取范围值随机经纬度集合
    static QStringList getRandPoint(int count, float mainLng, float mainLat, float dotLng, float dotLat);
    //根据旧的范围值和值计算新的范围值对应的值
    static int getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax);

    //获取uuid
    static QString getUuid();
    //校验目录
    static QString checkPath(const QString &dirName);
    //转换成完整路径
    static QString checkFile(const QString &fileName);
    //通用延时函数(支持Qt4 Qt5 Qt6)
    static void sleep(int msec, bool exec = true);
    //检查程序是否已经运行
    static void checkRun();

    //设置Qt自带样式
    static void setStyle();
    //设置字体
    static QFont addFont(const QString &fontFile, const QString &fontName);
    static void setFont(int fontSize = 12);
    //设置编码
    static void setCode(bool utf8 = true);
    //设置翻译文件
    static void setTranslator(const QString &qmFile);

    //动态设置权限
    static bool checkPermission(const QString &permission);
    //申请安卓权限
    static void initAndroidPermission();

    //一次性设置所有包括编码样式字体等
    static void initAll(bool utf8 = true, bool style = true, bool tabCenter = true, int fontSize = 13);
    //初始化main函数最前面执行的一段代码
    static void initMain(bool desktopSettingsAware = false, bool use96Dpi = false, bool logCritical = true);
    //初始化opengl类型(1=AA_UseDesktopOpenGL 2=AA_UseOpenGLES 3=AA_UseSoftwareOpenGL)
    static void initOpenGL(quint8 type = 0, bool checkCardEnable = false, bool checkVirtualSystem = false);

    //读取qss文件获取样式表内容
    static QString getStyle(const QString &qssFile);
    //设置qss文件到全局样式
    static void setStyle(const QString &qssFile);

    //执行命令行返回执行结果
    static QString doCmd(const QString &program, const QStringList &arguments, int timeout = 1000);
    //获取显卡是否被禁用
    static bool isVideoCardEnable();
    //获取是否在虚拟机环境
    static bool isVirtualSystem();

    //插入消息
    static bool replaceCRLF;
    static QVector<int> msgTypes;
    static QVector<QString> msgKeys;
    static QVector<QColor> msgColors;
    static QString appendMsg(QTextEdit *textEdit, int type, const QString &data,
                             int maxCount, int &currentCount,
                             bool clear = false, bool pause = false);

    //设置无边框
    static void setFramelessForm(QWidget *widgetMain, bool tool = false, bool top = false, bool menu = true);

    //弹出框
    static int showMessageBox(const QString &text, int type = 0, int closeSec = 0, bool exec = false);
    //弹出消息框
    static void showMessageBoxInfo(const QString &text, int closeSec = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &text, int closeSec = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &text);

    //为什么还要自定义对话框因为可控宽高和汉化对应文本等
    //初始化对话框文本
    static void initDialog(QFileDialog *dialog, const QString &title, const QString &acceptName,
                           const QString &dirName, bool native, int width, int height);
    //拿到对话框结果
    static QString getDialogResult(QFileDialog *dialog);
    //选择文件对话框
    static QString getOpenFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //保存文件对话框
    static QString getSaveFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //选择目录对话框
    static QString getExistingDirectory(const QString &dirName = QString(),
                                        bool native = false, int width = 900, int height = 600);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString getXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static quint8 getOrCode(const QByteArray &data);
    //计算校验码
    static quint8 getCheckCode(const QByteArray &data);

    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);
    //打开文件带提示框
    static void openFile(const QString &fileName, const QString &msg);

    //检查ini配置文件
    static bool checkIniFile(const QString &iniFile);

    //首尾截断字符串显示
    static QString cutString(const QString &text, int len, int left, int right, bool file, const QString &mid = "...");

    //传入图片尺寸和窗体区域及边框大小返回居中区域(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static QRect getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth = 2, int scaleMode = 0);
    //传入图片尺寸和窗体尺寸及缩放策略返回合适尺寸(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static void getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode = 0, bool fast = true);

    //毫秒数转时间 00:00
    static QString getTimeString(qint64 time);
    //用时时间转秒数
    static QString getTimeString(QElapsedTimer timer);
    //文件大小转 KB MB GB TB
    static QString getSizeString(quint64 size);

    //设置系统时间
    static void setSystemDateTime(const QString &year, const QString &month, const QString &day,
                                  const QString &hour, const QString &min, const QString &sec);
    //设置开机自启动
    static void runWithSystem(bool autoRun = true);
    static void runWithSystem(const QString &fileName, const QString &filePath, bool autoRun = true);

    //启动运行程序(已经在运行则不启动)
    static void start(const QString &path, const QString &name, bool bin = true);
};

#endif // QTHELPER_H
