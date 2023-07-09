#include "quihelper.h"
#include "qnetworkinterface.h"
#include "qnetworkproxy.h"

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
int QUIHelper::getScreenIndex()
{
    //需要对多个屏幕进行处理
    int screenIndex = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    int screenCount = qApp->screens().count();
#else
    int screenCount = qApp->desktop()->screenCount();
#endif

    if (screenCount > 1) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        for (int i = 0; i < screenCount; ++i) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
            if (qApp->screens().at(i)->geometry().contains(pos)) {
#else
            if (qApp->desktop()->screenGeometry(i).contains(pos)) {
#endif
                screenIndex = i;
                break;
            }
        }
    }
    return screenIndex;
}

QRect QUIHelper::getScreenRect(bool available)
{
    QRect rect;
    int screenIndex = QUIHelper::getScreenIndex();
    if (available) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        rect = qApp->screens().at(screenIndex)->availableGeometry();
#else
        rect = qApp->desktop()->availableGeometry(screenIndex);
#endif
    } else {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        rect = qApp->screens().at(screenIndex)->geometry();
#else
        rect = qApp->desktop()->screenGeometry(screenIndex);
#endif
    }
    return rect;
}

int QUIHelper::deskWidth()
{
    return getScreenRect().width();
}

int QUIHelper::deskHeight()
{
    return getScreenRect().height();
}

QSize QUIHelper::deskSize()
{
    return getScreenRect().size();
}

QWidget *QUIHelper::centerBaseForm = 0;
void QUIHelper::setFormInCenter(QWidget *form)
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

void QUIHelper::showForm(QWidget *form)
{
    setFormInCenter(form);
    form->show();

    //判断宽高是否超过了屏幕分辨率,超过了则最大化显示
    //qDebug() << TIMEMS << form->size() << deskSize();
    if (form->width() + 20 > deskWidth() || form->height() + 50 > deskHeight()) {
        QMetaObject::invokeMethod(form, "showMaximized", Qt::QueuedConnection);
    }
}

QString QUIHelper::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
        name.replace("_armeabi-v7a", "");
    }

    return name;
}

QString QUIHelper::appPath()
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

QStringList QUIHelper::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef Q_OS_WASM
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface  &netInterface, netInterfaces) {
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

QList<QColor> QUIHelper::colors = QList<QColor>();
QList<QColor> QUIHelper::getColorList()
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

QStringList QUIHelper::getColorNames()
{
    QList<QColor> colors = getColorList();
    QStringList colorNames;
    foreach (QColor color, colors) {
        colorNames << color.name();
    }
    return colorNames;
}

QColor QUIHelper::getRandColor()
{
    QList<QColor> colors = getColorList();
    int index = getRandValue(0, colors.count(), true);
    return colors.at(index);
}

void QUIHelper::initRand()
{
    //初始化随机数种子
    QTime t = QTime::currentTime();
    srand(t.msec() + t.second() * 1000);
}

float QUIHelper::getRandFloat(float min, float max)
{
    double diff = fabs(max - min);
    double value = (double)(rand() % 100) / 100;
    value = min + value * diff;
    return value;
}

double QUIHelper::getRandValue(int min, int max, bool contansMin, bool contansMax)
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

QStringList QUIHelper::getRandPoint(int count, float mainLng, float mainLat, float dotLng, float dotLat)
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

QString QUIHelper::getUuid()
{
    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "");
    uuid.replace("}", "");
    return uuid;
}

void QUIHelper::newDir(const QString &dirName)
{
    QString strDir = dirName;

    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!strDir.startsWith("/") && !strDir.contains(":/")) {
        strDir = QString("%1/%2").arg(QUIHelper::appPath()).arg(strDir);
    }

    QDir dir(strDir);
    if (!dir.exists()) {
        dir.mkpath(strDir);
    }
}

void QUIHelper::sleep(int msec)
{
    if (msec <= 0) {
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QThread::msleep(msec);
#else
    QTime endTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < endTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
}

void QUIHelper::setStyle()
{
    //打印下所有内置风格的名字
    qDebug() << TIMEMS << "QStyleFactory::keys" << QStyleFactory::keys();
    //设置内置风格
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qApp->setStyle(QStyleFactory::create("Fusion"));
#else
    qApp->setStyle(QStyleFactory::create("Cleanlooks"));
#endif

    //设置指定颜色
    QPalette palette;
    palette.setBrush(QPalette::Window, QColor("#F0F0F0"));
    qApp->setPalette(palette);
}

QFont QUIHelper::addFont(const QString &fontFile, const QString &fontName)
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

void QUIHelper::setFont(int fontSize)
{
#ifdef rk3399
    return;
#endif
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
#ifdef Q_OS_ANDROID
    fontSize = 15;
#endif

    QFont font;
    font.setFamily("MicroSoft Yahei");
    font.setPixelSize(fontSize);
    qApp->setFont(font);
}

void QUIHelper::setCode(bool utf8)
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

void QUIHelper::setTranslator(const QString &qmFile)
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

void QUIHelper::initAll(bool utf8, bool style, int fontSize)
{
    //初始化随机数种子
    QUIHelper::initRand();
    //设置编码
    QUIHelper::setCode(utf8);
    //设置样式风格
    if (style) {
        QUIHelper::setStyle();
    }
    //设置字体
    QUIHelper::setFont(fontSize);
    //设置翻译文件支持多个
    QUIHelper::setTranslator(":/qm/widgets.qm");
    QUIHelper::setTranslator(":/qm/qt_zh_CN.qm");
    QUIHelper::setTranslator(":/qm/designer_zh_CN.qm");
    //设置不使用本地系统环境代理配置
    QNetworkProxyFactory::setUseSystemConfiguration(false);
}

void QUIHelper::initMain(bool on)
{
    //设置是否应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(on);

#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //开启高分屏缩放支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    //不应用任何缩放
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    //高分屏缩放策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
    //设置opengl模式 AA_UseDesktopOpenGL(默认) AA_UseOpenGLES AA_UseSoftwareOpenGL
    //在一些很旧的设备上或者对opengl支持很低的设备上需要使用AA_UseOpenGLES表示禁用硬件加速
    //如果开启的是AA_UseOpenGLES则无法使用硬件加速比如ffmpeg的dxva2
    //QApplication::setAttribute(Qt::AA_UseOpenGLES);
    //设置opengl共享上下文
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif
}

QVector<int> QUIHelper::msgTypes = QVector<int>() << 0 << 1 << 2 << 3 << 4;
QVector<QString> QUIHelper::msgKeys = QVector<QString>() << "发送" << "接收" << "解析" << "错误" << "提示";
QVector<QColor> QUIHelper::msgColors = QVector<QColor>() << QColor("#3BA372") << QColor("#EE6668") << QColor("#9861B4") << QColor("#FA8359") << QColor("#22A3A9");
QString QUIHelper::appendMsg(QTextEdit *textEdit, int type, const QString &data, int maxCount, int &currentCount, bool clear, bool pause)
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

void QUIHelper::setFramelessForm(QWidget *widgetMain, bool tool, bool top, bool menu)
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

int QUIHelper::showMessageBox(const QString &info, int type, int closeSec, bool exec)
{
    int result = 0;
    if (type == 0) {
        showMessageBoxInfo(info, closeSec, exec);
    } else if (type == 1) {
        showMessageBoxError(info, closeSec, exec);
    } else if (type == 2) {
        result = showMessageBoxQuestion(info);
    }

    return result;
}

void QUIHelper::showMessageBoxInfo(const QString &info, int closeSec, bool exec)
{
    QMessageBox box(QMessageBox::Information, "提示", info);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::information(0, "提示", info, QMessageBox::Yes);
}

void QUIHelper::showMessageBoxError(const QString &info, int closeSec, bool exec)
{
    QMessageBox box(QMessageBox::Critical, "错误", info);
    box.setStandardButtons(QMessageBox::Yes);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.exec();
    //QMessageBox::critical(0, "错误", info, QMessageBox::Yes);
}

int QUIHelper::showMessageBoxQuestion(const QString &info)
{
    QMessageBox box(QMessageBox::Question, "询问", info);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setButtonText(QMessageBox::Yes, QString("确 定"));
    box.setButtonText(QMessageBox::No, QString("取 消"));
    return box.exec();
    //return QMessageBox::question(0, "询问", info, QMessageBox::Yes | QMessageBox::No);
}

void QUIHelper::initDialog(QFileDialog *dialog, const QString &title, const QString &acceptName,
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
        bool horizontal = (QUIHelper::deskWidth() > QUIHelper::deskHeight());
        if (horizontal) {
            width = QUIHelper::deskWidth() / 2;
            height = QUIHelper::deskHeight() - 50;
        } else {
            width = QUIHelper::deskWidth() - 10;
            height = QUIHelper::deskHeight() / 2;
        }
#endif
        dialog->setFixedSize(width, height);
    }

    //设置是否采用本地对话框
    dialog->setOption(QFileDialog::DontUseNativeDialog, !native);
    //设置只读可以取消右上角的新建按钮
    //dialog->setReadOnly(true);
}

QString QUIHelper::getDialogResult(QFileDialog *dialog)
{
    QString result;
    if (dialog->exec() == QFileDialog::Accepted) {
        result = dialog->selectedFiles().first();
    }
    return result;
}

QString QUIHelper::getOpenFileName(const QString &filter, const QString &dirName, const QString &fileName,
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

QString QUIHelper::getSaveFileName(const QString &filter, const QString &dirName, const QString &fileName,
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

QString QUIHelper::getExistingDirectory(const QString &dirName, bool native, int width, int height)
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

QString QUIHelper::getXorEncryptDecrypt(const QString &value, char key)
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

    int count = result.count();
    for (int i = 0; i < count; ++i) {
        result[i] = QChar(result.at(i).toLatin1() ^ key);
    }
    return result;
}

uchar QUIHelper::getOrCode(const QByteArray &data)
{
    int len = data.length();
    uchar result = 0;
    for (int i = 0; i < len; ++i) {
        result ^= data.at(i);
    }

    return result;
}

uchar QUIHelper::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    uchar temp = 0;
    for (uchar i = 0; i < len; ++i) {
        temp += data.at(i);
    }

    return temp % 256;
}

void QUIHelper::initTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast)
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

void QUIHelper::openFile(const QString &fileName, const QString &msg)
{
#ifdef __arm__
    return;
#endif
    //文件不存在则不用处理
    if (!QFile(fileName).exists()) {
        return;
    }
    if (QUIHelper::showMessageBoxQuestion(msg + "成功, 确定现在就打开吗?") == QMessageBox::Yes) {
        QString url = QString("file:///%1").arg(fileName);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

bool QUIHelper::checkIniFile(const QString &iniFile)
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

QString QUIHelper::cutString(const QString &text, int len, int left, int right, const QString &mid)
{
    //如果是文件名则取文件名的前字符+末尾字符+去掉拓展名
    QString result = text.split(".").first();
    if (result.length() > len) {
        result = QString("%1%2%3").arg(result.left(left)).arg(mid).arg(result.right(right));
    }
    return result;
}
