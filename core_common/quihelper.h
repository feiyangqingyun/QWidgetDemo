#ifndef QUIHELPER2_H
#define QUIHELPER2_H

#include "head.h"

class QUIHelper
{
public:
    //获取当前鼠标所在屏幕索引+尺寸
    static int getScreenIndex();
    static QRect getScreenRect(bool available = true);

    //获取桌面宽度高度+居中显示
    static int deskWidth();
    static int deskHeight();

    //居中显示窗体
    //定义标志位指定是以桌面为参照还是主程序界面为参照
    static QWidget *centerBaseForm;
    static void setFormInCenter(QWidget *form);

    //程序文件名称+当前所在路径
    static QString appName();
    static QString appPath();

    //获取uuid+初始化随机数种子+新建目录+延时
    static QString getUuid();
    static void initRand();
    static void newDir(const QString &dirName);
    static void sleep(int msec);

    //设置样式+字体+编码+居中+翻译
    static void setStyle();
    static void setFont(int fontSize = 12);    
    static void setCode(bool utf8 = true);
    static void setTranslator(const QString &qmFile = ":/image/qt_zh_CN.qm");

    //弹出框
    static int showMessageBox(const QString &info, int type = 0, int closeSec = 0, bool exec = false);
    //弹出消息框
    static void showMessageBoxInfo(const QString &info, int closeSec = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &info, int closeSec = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &info);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString getXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static uchar getOrCode(const QByteArray &data);
    //计算校验码
    static uchar getCheckCode(const QByteArray &data);

    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);
    //打开文件带提示框
    static void openFile(const QString &fileName, const QString &msg);

    //检查ini配置文件
    static bool checkIniFile(const QString &iniFile);
};

#endif // QUIHELPER2_H
