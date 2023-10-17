#include "qthelper.h"
#include "qnetworkinterface.h"
#include "qnetworkproxy.h"

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

QList<QRect> QtHelper::getScreenRects(bool available)
{
    QList<QRect> rects;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    int screenCount = qApp->screens().count();
    QList<QScreen *> screens = qApp->screens();
    for (int i = 0; i < screenCount; ++i) {
        QScreen *screen = screens.at(i);
        rects << (available ? screen->availableGeometry() : screen->geometry());
    }
#else
    int screenCount = qApp->desktop()->screenCount();
    QDesktopWidget *desk = qApp->desktop();
    for (int i = 0; i < screenCount; ++i) {
        rects << (available ? desk->availableGeometry(i) : desk->screenGeometry(i));
    }
#endif
    return rects;
}

int QtHelper::getScreenIndex()
{
    //需要对多个屏幕进行处理
    int screenIndex = 0;
    QList<QRect> rects = getScreenRects(false);
    int count = rects.count();
    for (int i = 0; i < count; ++i) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        if (rects.at(i).contains(pos)) {
            screenIndex = i;
            break;
        }
    }

    return screenIndex;
}

QRect QtHelper::getScreenRect(bool available)
{
    int screenIndex = getScreenIndex();
    QList<QRect> rects = getScreenRects(available);
    return rects.at(screenIndex);
}

qreal QtHelper::getScreenRatio(bool devicePixel)
{
    qreal ratio = 1.0;
    int screenIndex = getScreenIndex();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QScreen *screen = qApp->screens().at(screenIndex);
    if (devicePixel) {
        //需要开启 AA_EnableHighDpiScaling 属性才能正常获取
        ratio = screen->devicePixelRatio() * 100;
    } else {
        ratio = screen->logicalDotsPerInch();
    }
#else
    //Qt4不能动态识别缩放更改后的值
    ratio = qApp->desktop()->screen(screenIndex)->logicalDpiX();
#endif
    return ratio / 96;
}

QRect QtHelper::checkCenterRect(QRect &rect, bool available)
{
    QRect deskRect = QtHelper::getScreenRect(available);
    int formWidth = rect.width();
    int formHeight = rect.height();
    int deskWidth = deskRect.width();
    int deskHeight = deskRect.height();
    int formX = deskWidth / 2 - formWidth / 2 + deskRect.x();
    int formY = deskHeight / 2 - formHeight / 2;
    rect = QRect(formX, formY, formWidth, formHeight);
    return deskRect;
}

int QtHelper::deskWidth()
{
    return getScreenRect().width();
}

int QtHelper::deskHeight()
{
    return getScreenRect().height();
}

QSize QtHelper::deskSize()
{
    return getScreenRect().size();
}

QWidget *QtHelper::centerBaseForm = 0;
void QtHelper::setFormInCenter(QWidget *form)
{
    int formWidth = form->width();
    int formHeight = form->height();

    //如果=0表示采用系统桌面屏幕为参照
    QRect rect;
    if (centerBaseForm == 0) {
        rect = getScreenRect();
    } else {
        rect = centerBaseForm->geometry();
    }

    int deskWidth = rect.width();
    int deskHeight = rect.height();
    QPoint movePoint(deskWidth / 2 - formWidth / 2 + rect.x(), deskHeight / 2 - formHeight / 2 + rect.y());
    form->move(movePoint);
}

void QtHelper::showForm(QWidget *form)
{
    setFormInCenter(form);
    form->show();

    //判断宽高是否超过了屏幕分辨率,超过了则最大化显示
    //qDebug() << TIMEMS << form->size() << deskSize();
    if (form->width() + 20 > deskWidth() || form->height() + 50 > deskHeight()) {
        QMetaObject::invokeMethod(form, "showMaximized", Qt::QueuedConnection);
    }
}

QString QtHelper::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a/lib程序名_arm64-v8a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
        name.replace("_armeabi-v7a", "");
        name.replace("_arm64-v8a", "");
    }

    return name;
}

QString QtHelper::appPath()
{
    static QString path;
    if (path.isEmpty()) {
#ifdef Q_OS_ANDROID
        //默认安卓根目录
        path = "/storage/emulated/0";
        //带上程序名称作为目录 前面加个0方便排序
        path = path + "/0" + appName();
#else
        path = qApp->applicationDirPath();
#endif
    }

    return path;
}

void QtHelper::getCurrentInfo(char *argv[], QString &path, QString &name)
{
    //必须用fromLocal8Bit保证中文路径正常
    QString argv0 = QString::fromLocal8Bit(argv[0]);
    QFileInfo file(argv0);
    path = file.path();
    name = file.baseName();
}

QString QtHelper::getIniValue(const QString &fileName, const QString &key)
{
    QString value;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            if (line.startsWith(key)) {
                line = line.replace("\n", "");
                line = line.trimmed();
                value = line.split("=").last();
                break;
            }
        }
    }
    return value;
}

QString QtHelper::getIniValue(char *argv[], const QString &key, const QString &dir)
{
    QString path, name;
    QtHelper::getCurrentInfo(argv, path, name);
    QString fileName = QString("%1/%2%3.ini").arg(path).arg(dir).arg(name);
    return getIniValue(fileName, key);
}

QStringList QtHelper::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef Q_OS_WASM
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface netInterface, netInterfaces) {
            //移除虚拟机和抓包工具的虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") || humanReadableName.startsWith("npcap loopback adapter")) {
                continue;
            }

            //过滤当前网络接口
            bool flag = (netInterface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast | QNetworkInterface::CanMulticast));
            if (!flag) {
                continue;
            }

            QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
            foreach (QNetworkAddressEntry addr, addrs) {
                //只取出IPV4的地址
                if (addr.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                    continue;
                }

                QString ip4 = addr.ip().toString();
                if (ip4 != "127.0.0.1") {
                    ips << ip4;
                }
            }
        }
#endif
    }

    return ips;
}

QList<QColor> QtHelper::colors = QList<QColor>();
QList<QColor> QtHelper::getColorList()
{
    //备用颜色集合 可以自行添加
    if (colors.count() == 0) {
        colors << QColor(0, 176, 180) << QColor(0, 113, 193) << QColor(255, 192, 0);
        colors << QColor(72, 103, 149) << QColor(185, 87, 86) << QColor(0, 177, 125);
        colors << QColor(214, 77, 84) << QColor(71, 164, 233) << QColor(34, 163, 169);
        colors << QColor(59, 123, 156) << QColor(162, 121, 197) << QColor(72, 202, 245);
        colors << QColor(0, 150, 121) << QColor(111, 9, 176) << QColor(250, 170, 20);
    }

    return colors;
}

QStringList QtHelper::getColorNames()
{
    QList<QColor> colors = getColorList();
    QStringList colorNames;
    foreach (QColor color, colors) {
        colorNames << color.name();
    }
    return colorNames;
}

QColor QtHelper::getRandColor()
{
    QList<QColor> colors = getColorList();
    int index = getRandValue(0, colors.count(), true);
    return colors.at(index);
}

void QtHelper::initRand()
{
    //初始化随机数种子
    QTime t = QTime::currentTime();
    srand(t.msec() + t.second() * 1000);
}

float QtHelper::getRandFloat(float min, float max)
{
    double diff = fabs(max - min);
    double value = (double)(rand() % 100) / 100;
    value = min + value * diff;
    return value;
}

double QtHelper::getRandValue(int min, int max, bool contansMin, bool contansMax)
{
    int value;
#if (QT_VERSION <= QT_VERSION_CHECK(5,10,0))
    //通用公式 a是起始值,n是整数的范围
    //int value = a + rand() % n;
    if (contansMin) {
        if (contansMax) {
            value = min + 0 + (rand() % (max - min + 1));
        } else {
            value = min + 0 + (rand() % (max - min + 0));
        }
    } else {
        if (contansMax) {
            value = min + 1 + (rand() % (max - min + 0));
        } else {
            value = min + 1 + (rand() % (max - min - 1));
        }
    }
#else
    if (contansMin) {
        if (contansMax) {
            value = QRandomGenerator::global()->bounded(min + 0, max + 1);
        } else {
            value = QRandomGenerator::global()->bounded(min + 0, max + 0);
        }
    } else {
        if (contansMax) {
            value = QRandomGenerator::global()->bounded(min + 1, max + 1);
        } else {
            value = QRandomGenerator::global()->bounded(min + 1, max + 0);
        }
    }
#endif
    return value;
}

QStringList QtHelper::getRandPoint(int count, float mainLng, float mainLat, float dotLng, float dotLat)
{
    //随机生成点坐标
    QStringList points;
    for (int i = 0; i < count; ++i) {
        //0.00881415 0.000442928
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0))
        float lngx = QRandomGenerator::global()->bounded(dotLng);
        float latx = QRandomGenerator::global()->bounded(dotLat);
#else
        float lngx = getRandFloat(dotLng / 10, dotLng);
        float latx = getRandFloat(dotLat / 10, dotLat);
#endif
        //需要先用精度转换成字符串
        QString lng2 = QString::number(mainLng + lngx, 'f', 8);
        QString lat2 = QString::number(mainLat + latx, 'f', 8);
        QString point = QString("%1,%2").arg(lng2).arg(lat2);
        points << point;
    }

    return points;
}

int QtHelper::getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax)
{
    return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
}

QString QtHelper::getUuid()
{
    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "");
    uuid.replace("}", "");
    return uuid;
}

void QtHelper::checkPath(const QString &dirName)
{
    //相对路径需要补全完整路径
    QString path = dirName;
    if (path.startsWith("./")) {
        path.replace(".", "");
        path = QtHelper::appPath() + path;
    } else if (!path.startsWith("/") && !path.contains(":/")) {
        path = QtHelper::appPath() + "/" + path;
    }

    //目录不存在则新建
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }
}

void QtHelper::sleep(int msec, bool exec)
{
    if (msec <= 0) {
        return;
    }

    if (exec) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        //阻塞方式延时(如果在主线程会卡住主界面)
        QThread::msleep(msec);
#else
        //非阻塞方式延时(不会卡住主界面/据说可能有问题)
        QTime endTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < endTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
#endif
    } else {
        //非阻塞方式延时(现在很多人推荐的方法)
        QEventLoop loop;
        QTimer::singleShot(msec, &loop, SLOT(quit()));
        loop.exec();
    }
}

void QtHelper::checkRun()
{
#ifdef Q_OS_WIN
    //延时1秒钟,等待程序释放完毕
    QtHelper::sleep(1000);
    //创建共享内存,判断是否已经运行程序
    static QSharedMemory mem(QtHelper::appName());
    if (!mem.create(1)) {
        QtHelper::showMessageBoxError("程序已运行, 软件将自动关闭!", 5, true);
        exit(0);
    }
#endif
}

void QtHelper::setStyle()
{
    //打印下所有内置风格的名字
    qDebug() << TIMEMS << "QStyleFactory::keys" << QStyleFactory::keys();
    //设置内置风格
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qApp->setStyle("Fusion");
#else
    qApp->setStyle("Cleanlooks");
#endif

    //设置指定颜色
    QPalette palette;
    palette.setBrush(QPalette::Window, QColor("#F0F0F0"));
    qApp->setPalette(palette);
}

QFont QtHelper::addFont(const QString &fontFile, const QString &fontName)
{
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains(fontName)) {
        int fontId = fontDb.addApplicationFont(fontFile);
        QStringList listName = fontDb.applicationFontFamilies(fontId);
        if (listName.count() == 0) {
            qDebug() << QString("load %1 error").arg(fontName);
        }
    }

    //再次判断是否包含字体名称防止加载失败
    QFont font;
    if (fontDb.families().contains(fontName)) {
        font = QFont(fontName);
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        font.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    return font;
}

void QtHelper::setFont(int fontSize)
{
    //安卓套件在有些手机上默认字体不好看需要主动设置字体
    //网页套件需要主动加载中文字体才能正常显示中文
#if (defined Q_OS_ANDROID) || (defined Q_OS_WASM)
    QString fontFile = ":/font/DroidSansFallback.ttf";
    QString fontName = "Droid Sans Fallback";
    qApp->setFont(addFont(fontFile, fontName));
    return;
#endif

#ifdef __arm__
    fontSize = 25;
#endif

    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPixelSize(fontSize);
    qApp->setFont(font);
}

void QtHelper::setCode(bool utf8)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    //如果想要控制台打印信息中文正常就注释掉这个设置
    if (utf8) {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    }
#else
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

void QtHelper::setTranslator(const QString &qmFile)
{
    //过滤下不存在的就不用设置了
    if (!QFile(qmFile).exists()) {
        return;
    }

    QTranslator *translator = new QTranslator(qApp);
    if (translator->load(qmFile)) {
        qApp->installTranslator(translator);
    }
}

#ifdef Q_OS_ANDROID
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
#include <QtAndroidExtras>
#else
//Qt6中将相关类移到了core模块而且名字变了
#include <QtCore/private/qandroidextras_p.h>
#endif
#endif

bool QtHelper::checkPermission(const QString &permission)
{
#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0) && QT_VERSION < QT_VERSION_CHECK(6,0,0))
    QtAndroid::PermissionResult result = QtAndroid::checkPermission(permission);
    if (result == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList() << permission);
        result = QtAndroid::checkPermission(permission);
        if (result == QtAndroid::PermissionResult::Denied) {
            return false;
        }
    }
#else
    QFuture<QtAndroidPrivate::PermissionResult> result = QtAndroidPrivate::requestPermission(permission);
    if (result.resultAt(0) == QtAndroidPrivate::PermissionResult::Denied) {
        return false;
    }
#endif
#endif
    return true;
}

void QtHelper::initAndroidPermission()
{
    //可以把所有要动态申请的权限都写在这里
    checkPermission("android.permission.CALL_PHONE");
    checkPermission("android.permission.SEND_SMS");
    checkPermission("android.permission.CAMERA");
    checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");

    checkPermission("android.permission.ACCESS_COARSE_LOCATION");
    checkPermission("android.permission.BLUETOOTH");
    checkPermission("android.permission.BLUETOOTH_SCAN");
    checkPermission("android.permission.BLUETOOTH_CONNECT");
    checkPermission("android.permission.BLUETOOTH_ADVERTISE");
}

void QtHelper::initAll(bool utf8, bool style, int fontSize)
{
    //初始化安卓权限
    QtHelper::initAndroidPermission();
    //初始化随机数种子
    QtHelper::initRand();
    //设置编码
    QtHelper::setCode(utf8);
    //设置字体
    QtHelper::setFont(fontSize);
    //设置样式风格
    if (style) {
        QtHelper::setStyle();
    }

    //设置翻译文件支持多个
    QtHelper::setTranslator(":/qm/widgets.qm");
    QtHelper::setTranslator(":/qm/qt_zh_CN.qm");
    QtHelper::setTranslator(":/qm/designer_zh_CN.qm");

    //设置不使用本地系统环境代理配置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    //设置当前目录为程序可执行文件所在目录
    QDir::setCurrent(QtHelper::appPath());
    //Qt4中默认没有程序名称需要主动设置
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    qApp->setApplicationName(QtHelper::appName());
#endif
}

void QtHelper::initMain(bool desktopSettingsAware, bool use96Dpi, bool logCritical)
{
#ifdef Q_OS_WIN
    //Qt6.5开始默认是ffmpeg后端但是不成熟需要换成系统的
    qputenv("QT_MEDIA_BACKEND", "windows");
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    //设置是否应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(desktopSettingsAware);
#endif

    bool highDpi = !use96Dpi;
#ifdef Q_OS_ANDROID
    highDpi = true;
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //开启高分屏缩放支持
    if (highDpi) {
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
#endif

#ifdef Q_OS_WIN
    if (use96Dpi) {
        //Qt6中AA_Use96Dpi没效果必须下面方式设置强制指定缩放DPI
        qputenv("QT_FONT_DPI", "96");
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        //不应用任何缩放
        QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
    }
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    //高分屏缩放策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    //下面这行表示不打印Qt内部类的警告提示信息
    if (!logCritical) {
        QLoggingCategory::setFilterRules("*.critical=false\n*.warning=false");
    }
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
    //设置opengl共享上下文
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
}

void QtHelper::initOpenGL(quint8 type, bool checkCardEnable, bool checkVirtualSystem)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
    //设置opengl模式 AA_UseDesktopOpenGL(默认) AA_UseOpenGLES AA_UseSoftwareOpenGL
    //在一些很旧的设备上或者对opengl支持很低的设备上需要使用AA_UseOpenGLES表示禁用硬件加速
    //如果开启的是AA_UseOpenGLES则无法使用硬件加速比如ffmpeg的dxva2
    if (type == 1) {
        QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    } else if (type == 2) {
        QApplication::setAttribute(Qt::AA_UseOpenGLES);
    } else if (type == 3) {
        QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    }

    //检测显卡是否被禁用
    if (checkCardEnable && !isVideoCardEnable()) {
        QApplication::setAttribute(Qt::AA_UseOpenGLES);
    }

    //检测是否是虚拟机系统
    if (checkVirtualSystem && isVirtualSystem()) {
        QApplication::setAttribute(Qt::AA_UseOpenGLES);
    }
#endif
}

QString QtHelper::doCmd(const QString &program, const QStringList &arguments, int timeout)
{
    QString result;
#ifndef Q_OS_WASM
    QProcess p;
    p.start(program, arguments);
    p.waitForFinished(timeout);
    result = QString::fromLocal8Bit(p.readAllStandardOutput());
    result.replace("\r", "");
    result.replace("\n", "");
    result = result.simplified();
    result = result.trimmed();
#endif
    return result;
}

bool QtHelper::isVideoCardEnable()
{
    QString result;
    bool videoCardEnable = true;

#if defined(Q_OS_WIN)
    QStringList args;
    //wmic path win32_VideoController get name,Status
    args << "path" << "win32_VideoController" << "get" << "name,Status";
    result = doCmd("wmic", args);
#endif

    //Name Status Intel(R) UHD Graphics 630 OK
    //Name Status Intel(R) UHD Graphics 630 Error
    if (result.contains("Error")) {
        videoCardEnable = false;
    }

    return videoCardEnable;
}

bool QtHelper::isVirtualSystem()
{
    QString result;
    bool virtualSystem = false;

#if defined(Q_OS_WIN)
    QStringList args;
    //wmic computersystem get Model
    args << "computersystem" << "get" << "Model";
    result = doCmd("wmic", args);
#elif defined(Q_OS_LINUX)
    QStringList args;
    //还有个命令需要root权限运行 dmidecode -s system-product-name 执行结果和win一样
    result = doCmd("lscpu", args);
#endif

    //Model MS-7C00
    //Model VMWare Virtual Platform
    //Model VirtualBox Virtual Platform
    //Model Alibaba Cloud ECS
    if (result.contains("VMware") || result.contains("VirtualBox") || result.contains("Alibaba")) {
        virtualSystem = true;
    }

    return virtualSystem;
}

QVector<int> QtHelper::msgTypes = QVector<int>() << 0 << 1 << 2 << 3 << 4;
QVector<QString> QtHelper::msgKeys = QVector<QString>() << QString::fromUtf8("发送") << QString::fromUtf8("接收") << QString::fromUtf8("解析") << QString::fromUtf8("错误") << QString::fromUtf8("提示");
QVector<QColor> QtHelper::msgColors = QVector<QColor>() << QColor("#3BA372") << QColor("#EE6668") << QColor("#9861B4") << QColor("#FA8359") << QColor("#22A3A9");
QString QtHelper::appendMsg(QTextEdit *textEdit, int type, const QString &data, int maxCount, int &currentCount, bool clear, bool pause)
{
    if (clear) {
        textEdit->clear();
        currentCount = 0;
        return QString();
    }

    if (pause) {
        return QString();
    }

    if (currentCount >= maxCount) {
        textEdit->clear();
        currentCount = 0;
    }

    //不同类型不同颜色显示
    QString strType;
    int index = msgTypes.indexOf(type);
    if (index >= 0) {
        strType = msgKeys.at(index);
        textEdit->setTextColor(msgColors.at(index));
    }

    //过滤回车换行符
    QString strData = data;
    strData.replace("\r", "");
    strData.replace("\n", "");
    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    textEdit->append(strData);
    currentCount++;
    return strData;
}

void QtHelper::setFramelessForm(QWidget *widgetMain, bool tool, bool top, bool menu)
{
    widgetMain->setProperty("form", true);
    widgetMain->setProperty("canMove", true);

    //根据设定逐个追加属性
#ifdef __arm__
    widgetMain->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    widgetMain->setWindowFlags(Qt::FramelessWindowHint);
#endif
    if (tool) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::Tool);
    }
    if (top) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    if (menu) {
        //如果是其他系统比如neokylin会产生系统边框
#ifdef Q_OS_WIN
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
#endif
    }
}

int QtHelper::showMessageBox(const QString &text, int type, int closeSec, bool exec)
{
    int result = 0;
    if (type == 0) {
        showMessageBoxInfo(text, closeSec, exec);
    } else if (type == 1) {
        showMessageBoxError(text, closeSec, exec);
    } else if (type == 2) {
        result = showMessageBoxQuestion(text);
    }

    return result;
}

void QtHelper::showMessageBoxInfo(const QString &text, int closeSec, bool exec)
{
    QMessageBox box(QMessageBox::Information, "提示", text);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::information(0, "提示", info, QMessageBox::Yes);
}

void QtHelper::showMessageBoxError(const QString &text, int closeSec, bool exec)
{
    QMessageBox box(QMessageBox::Critical, "错误", text);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::critical(0, "错误", info, QMessageBox::Yes);
}

int QtHelper::showMessageBoxQuestion(const QString &text)
{
    QMessageBox box(QMessageBox::Question, "询问", text);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.setButtonText(QMessageBox::No, QString("取 消"));
    return box.exec();
    //return QMessageBox::question(0, "询问", info, QMessageBox::Yes | QMessageBox::No);
}

void QtHelper::initDialog(QFileDialog *dialog, const QString &title, const QString &acceptName,
                          const QString &dirName, bool native, int width, int height)
{
    //设置标题
    dialog->setWindowTitle(title);
    //设置标签文本
    dialog->setLabelText(QFileDialog::Accept, acceptName);
    dialog->setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog->setLabelText(QFileDialog::LookIn, "查看");
    dialog->setLabelText(QFileDialog::FileName, "名称");
    dialog->setLabelText(QFileDialog::FileType, "类型");

    //设置默认显示目录
    if (!dirName.isEmpty()) {
        dialog->setDirectory(dirName);
    }

    //设置对话框宽高
    if (width > 0 && height > 0) {
#ifdef Q_OS_ANDROID
        bool horizontal = (QtHelper::deskWidth() > QtHelper::deskHeight());
        if (horizontal) {
            width = QtHelper::deskWidth() / 2;
            height = QtHelper::deskHeight() - 50;
        } else {
            width = QtHelper::deskWidth() - 10;
            height = QtHelper::deskHeight() / 2;
        }
#endif
        dialog->setFixedSize(width, height);
    }

    //设置是否采用本地对话框
    dialog->setOption(QFileDialog::DontUseNativeDialog, !native);
    //设置只读可以取消右上角的新建按钮
    //dialog->setReadOnly(true);
}

QString QtHelper::getDialogResult(QFileDialog *dialog)
{
    QString result;
    if (dialog->exec() == QFileDialog::Accepted) {
        result = dialog->selectedFiles().first();
        if (!result.contains(".")) {
            //自动补全拓展名 保存文件(*.txt *.exe)
            QString filter = dialog->selectedNameFilter();
            if (filter.contains("*.")) {
                filter = filter.split("(").last();
                filter = filter.mid(0, filter.length() - 1);
                //取出第一个作为拓展名
                if (!filter.contains("*.*")) {
                    filter = filter.split(" ").first();
                    result = result + filter.mid(1, filter.length());
                }
            }
        }
    }
    return result;
}

QString QtHelper::getOpenFileName(const QString &filter, const QString &dirName, const QString &fileName,
                                  bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "打开文件", "选择(&S)", dirName, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(fileName);
    return getDialogResult(&dialog);
}

QString QtHelper::getSaveFileName(const QString &filter, const QString &dirName, const QString &fileName,
                                  bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "保存文件", "保存(&S)", dirName, native, width, height);

    //设置文件类型
    if (!filter.isEmpty()) {
        dialog.setNameFilter(filter);
    }

    //设置默认文件名称
    dialog.selectFile(fileName);
    //设置模态类型允许输入
    dialog.setWindowModality(Qt::WindowModal);
    //设置置顶显示
    dialog.setWindowFlags(dialog.windowFlags() | Qt::WindowStaysOnTopHint);
    return getDialogResult(&dialog);
}

QString QtHelper::getExistingDirectory(const QString &dirName, bool native, int width, int height)
{
    QFileDialog dialog;
    initDialog(&dialog, "选择目录", "选择(&S)", dirName, native, width, height);
    dialog.setOption(QFileDialog::ReadOnly);
    //设置只显示目录
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    dialog.setFileMode(QFileDialog::DirectoryOnly);
#endif
    dialog.setOption(QFileDialog::ShowDirsOnly);
    return getDialogResult(&dialog);
}

QString QtHelper::getXorEncryptDecrypt(const QString &value, char key)
{
    //矫正范围外的数据
    if (key < 0 || key >= 127) {
        key = 127;
    }

    //大概从5.9版本输出的加密密码字符串前面会加上 @String 字符
    QString result = value;
    if (result.startsWith("@String")) {
        result = result.mid(8, result.length() - 9);
    }

    for (int i = 0; i < result.length(); ++i) {
        result[i] = QChar(result.at(i).toLatin1() ^ key);
    }
    return result;
}

quint8 QtHelper::getOrCode(const QByteArray &data)
{
    int len = data.length();
    quint8 result = 0;
    for (int i = 0; i < len; ++i) {
        result ^= data.at(i);
    }

    return result;
}

quint8 QtHelper::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    quint8 temp = 0;
    for (int i = 0; i < len; ++i) {
        temp += data.at(i);
    }

    return temp % 256;
}

void QtHelper::initTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast)
{
    //设置弱属性用于应用qss特殊样式
    tableView->setProperty("model", true);
    //取消自动换行
    tableView->setWordWrap(false);
    //超出文本不显示省略号
    tableView->setTextElideMode(Qt::ElideNone);
    //奇数偶数行颜色交替
    tableView->setAlternatingRowColors(false);
    //垂直表头是否可见
    tableView->verticalHeader()->setVisible(headVisible);
    //选中一行表头是否加粗
    tableView->horizontalHeader()->setHighlightSections(false);
    //最后一行拉伸填充
    tableView->horizontalHeader()->setStretchLastSection(stretchLast);
    //行标题最小宽度尺寸
    tableView->horizontalHeader()->setMinimumSectionSize(0);
    //行标题最小高度,等同于和默认行高一致
    tableView->horizontalHeader()->setFixedHeight(rowHeight);
    //默认行高
    tableView->verticalHeader()->setDefaultSectionSize(rowHeight);
    //选中时一行整体选中
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只允许选择单个
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //表头不可单击
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    tableView->horizontalHeader()->setSectionsClickable(false);
#else
    tableView->horizontalHeader()->setClickable(false);
#endif

    //鼠标按下即进入编辑模式
    if (edit) {
        tableView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked);
    } else {
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void QtHelper::openFile(const QString &fileName, const QString &msg)
{
#ifdef __arm__
    return;
#endif
    //文件不存在则不用处理
    if (!QFile(fileName).exists()) {
        return;
    }
    if (QtHelper::showMessageBoxQuestion(msg + "成功, 确定现在就打开吗?") == QMessageBox::Yes) {
        QString url = QString("file:///%1").arg(fileName);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

bool QtHelper::checkIniFile(const QString &iniFile)
{
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(iniFile);
    if (file.size() == 0) {
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.replace("\r", "");
            line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                QString key = list.at(0);
                QString value = list.at(1);
                if (value.isEmpty()) {
                    qDebug() << TIMEMS << "ini node no value" << key;
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

QString QtHelper::cutString(const QString &text, int len, int left, int right, bool file, const QString &mid)
{
    //如果指定了字符串分割则表示是文件名需要去掉拓展名
    QString result = text;
    if (file && result.contains(".")) {
        int index = result.lastIndexOf(".");
        result = result.mid(0, index);
    }

    //最终字符串格式为 前缀字符...后缀字符
    if (result.length() > len) {
        result = QString("%1%2%3").arg(result.left(left)).arg(mid).arg(result.right(right));
    }

    return result;
}

QRect QtHelper::getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth, int scaleMode)
{
    QSize newSize = imageSize;
    QSize widgetSize = widgetRect.size() - QSize(borderWidth * 1, borderWidth * 1);

    if (scaleMode == 0) {
        if (newSize.width() > widgetSize.width() || newSize.height() > widgetSize.height()) {
            newSize.scale(widgetSize, Qt::KeepAspectRatio);
        }
    } else if (scaleMode == 1) {
        newSize.scale(widgetSize, Qt::KeepAspectRatio);
    } else {
        newSize = widgetSize;
    }

    int x = widgetRect.center().x() - newSize.width() / 2;
    int y = widgetRect.center().y() - newSize.height() / 2;
    //不是2的倍数需要偏移1像素
    x += (x % 2 == 0 ? 1 : 0);
    y += (y % 2 == 0 ? 1 : 0);
    return QRect(x, y, newSize.width(), newSize.height());
}

void QtHelper::getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode, bool fast)
{
    Qt::TransformationMode mode = fast ? Qt::FastTransformation : Qt::SmoothTransformation;
    if (scaleMode == 0) {
        if (image.width() > widgetSize.width() || image.height() > widgetSize.height()) {
            image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
        }
    } else if (scaleMode == 1) {
        image = image.scaled(widgetSize, Qt::KeepAspectRatio, mode);
    } else {
        image = image.scaled(widgetSize, Qt::IgnoreAspectRatio, mode);
    }
}

QString QtHelper::getTimeString(qint64 time)
{
    time = time / 1000;
    QString min = QString("%1").arg(time / 60, 2, 10, QChar('0'));
    QString sec = QString("%2").arg(time % 60, 2, 10, QChar('0'));
    return QString("%1:%2").arg(min).arg(sec);
}

QString QtHelper::getTimeString(QElapsedTimer timer)
{
    return QString::number((float)timer.elapsed() / 1000, 'f', 3);
}

QString QtHelper::getSizeString(quint64 size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QString unit("bytes");
    QStringListIterator i(list);
    while (num >= 1024.0 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    return QString("%1 %2").arg(QString::number(num, 'f', 2)).arg(unit);
}

//setSystemDateTime("2022", "07", "01", "12", "22", "55");
void QtHelper::setSystemDateTime(const QString &year, const QString &month, const QString &day, const QString &hour, const QString &min, const QString &sec)
{
#ifdef Q_OS_WIN
    QProcess p(0);
    //先设置日期
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
    //再设置时间
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
#else
    QString cmd = QString("date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
    //设置日期时间
    system(cmd.toLatin1());
    //硬件时钟同步
    system("hwclock -w");
#endif
}

void QtHelper::runWithSystem(bool autoRun)
{
    QtHelper::runWithSystem(qApp->applicationName(), qApp->applicationFilePath(), autoRun);
}

void QtHelper::runWithSystem(const QString &fileName, const QString &filePath, bool autoRun)
{
#ifdef Q_OS_WIN
    //要转换成本地文件路径(不启动则文件路径为空即可)
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(fileName, autoRun ? QDir::toNativeSeparators(filePath) : "");
#endif
}

void QtHelper::runBin(const QString &path, const QString &name)
{
#ifdef Q_OS_WIN
    QString cmd1 = "tasklist";
    QString cmd2 = QString("%1/%2.exe").arg(path).arg(name);
#else
    QString cmd1 = "ps -aux";
    QString cmd2 = QString("%1/%2").arg(path).arg(name);
#endif

#ifndef Q_OS_WASM
    QProcess p;
    p.start(cmd1);
    if (p.waitForFinished()) {
        QString result = p.readAll();
        if (result.contains(name)) {
            return;
        }
    }

    //加上引号可以兼容打开带空格的目录(Program Files)
    if (cmd2.contains(" ")) {
        cmd2 = "\"" + cmd2 + "\"";
    }

    //QProcess::execute(cmd2);
    QProcess::startDetached(cmd2);
#endif
}
