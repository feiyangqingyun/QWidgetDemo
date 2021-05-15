#ifndef QUIHELPER_H
#define QUIHELPER_H

#include "quihead.h"

class QUIHelper
{
public:
    //获取uuid
    static QString getUuid();
    //获取当前鼠标所在屏幕
    static int getScreenIndex();

    //桌面宽度高度
    static int deskWidth();
    static int deskHeight();

    //程序文件名称+当前所在路径
    static QString appName();
    static QString appPath();

    //初始化随机数种子
    static void initRand();

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
    static void setFramelessForm(QWidget *widgetMain,
                                 bool tool = false, bool top = false, bool menu = true);
    static void setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                 QLabel *labIco, QPushButton *btnClose,
                                 bool tool = true, bool top = true, bool menu = false);

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
    static quint16 getCRC16Rec(quint8 *data, int len, quint16 init, const quint16 *table);
    static quint16 getCRC16(quint8 *data, int len, quint16 init, const quint16 *table);
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

    //定义标志位启用系统的还是自定义的对话框
    static bool isCustomUI;
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


    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);
    //打开文件带提示框
    static void openFile(const QString &fileName, const QString &msg);
    //导出和打印数据提示框
    static bool checkRowCount(int rowCount, int maxCount, int warnCount);
};

#endif // QUIHELPER_H
