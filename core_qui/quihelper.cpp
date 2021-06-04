#include "quihelper.h"

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

void QUIHelper::setFormInCenter(QWidget *form)
{
    int formWidth = form->width();
    int formHeight = form->height();
    QRect rect = getScreenRect();
    int deskWidth = rect.width();
    int deskHeight = rect.height();
    QPoint movePoint(deskWidth / 2 - formWidth / 2 + rect.x(), deskHeight / 2 - formHeight / 2);
    form->move(movePoint);

    //其他系统自动最大化
#ifndef Q_OS_WIN
    QTimer::singleShot(100, form, SLOT(showMaximized()));
#endif
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
    }

    return name;
}

QString QUIHelper::appPath()
{
#ifdef Q_OS_ANDROID
    //return QString("/sdcard/Android/%1").arg(appName());
    return QString("/storage/emulated/0/%1").arg(appName());
#else
    return qApp->applicationDirPath();
#endif
}

QString QUIHelper::getUuid()
{
    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.replace("{", "");
    uuid = uuid.replace("}", "");
    return uuid;
}

void QUIHelper::initRand()
{
    //初始化随机数种子
    QTime t = QTime::currentTime();
    srand(t.msec() + t.second() * 1000);
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
    if (msec > 0) {
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        QTime endTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < endTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
#else
        QThread::msleep(msec);
#endif
    }
}

void QUIHelper::setCode(bool utf8)
{
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    //如果想要控制台打印信息中文正常就注释掉这个设置
    if (utf8) {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    }
#endif
}

void QUIHelper::setFont(const QString &ttfFile, const QString &fontName, int fontSize)
{
    QFont font;
    font.setFamily(fontName);
    font.setPixelSize(fontSize);

    //如果存在字体文件则设备字体文件中的字体
    //安卓版本和网页版本需要字体文件一起打包单独设置字体
    if (!ttfFile.isEmpty()) {
        QFontDatabase fontDb;
        int fontId = fontDb.addApplicationFont(ttfFile);
        if (fontId != -1) {
            QStringList androidFont = fontDb.applicationFontFamilies(fontId);
            if (androidFont.size() != 0) {
                font.setFamily(androidFont.at(0));
                font.setPixelSize(fontSize);
            }
        }
    }
    qApp->setFont(font);
}

void QUIHelper::setTranslator(const QString &qmFile)
{
    QTranslator *translator = new QTranslator(qApp);
    translator->load(qmFile);
    qApp->installTranslator(translator);
}

void QUIHelper::initDb(const QString &dbName)
{
    initFile(QString(":/%1.db").arg(appName()), dbName);
}

void QUIHelper::initFile(const QString &sourceName, const QString &targetName)
{
    //判断文件是否存在,不存在则从资源文件复制出来
    QFile file(targetName);
    if (!file.exists() || file.size() == 0) {
        file.remove();
        QUIHelper::copyFile(sourceName, targetName);
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
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    qDebug() << TIMEMS << "ini node no value" << list.at(0);
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

void QUIHelper::setIconBtn(QAbstractButton *btn, const QString &png, int icon)
{
    int size = 16;
    int width = 18;
    int height = 18;
    QPixmap pix;
    if (QPixmap(png).isNull()) {
        pix = IconHelper::Instance()->getPixmap(QUIConfig::TextColor, icon, size, width, height);
    } else {
        pix = QPixmap(png);
    }

    btn->setIconSize(QSize(width, height));
    btn->setIcon(QIcon(pix));
}

void QUIHelper::writeInfo(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runinfo_%4.txt").arg(QUIHelper::appPath())
                       .arg(filePath).arg(QUIHelper::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}

void QUIHelper::writeError(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runerror_%4.txt").arg(QUIHelper::appPath())
                       .arg(filePath).arg(QUIHelper::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}

void QUIHelper::setFramelessForm(QWidget *widgetMain, bool tool, bool top, bool menu)
{
    widgetMain->setProperty("form", true);
    widgetMain->setProperty("canMove", true);

    //根据设定逐个追加属性
    widgetMain->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    if (tool) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::Tool);
    }
    if (top) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    if (menu) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    }
}

void QUIHelper::setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                 QLabel *labIco, QPushButton *btnClose,
                                 bool tool, bool top, bool menu)
{
    labIco->setFixedWidth(QUITitleMinSize);
    btnClose->setFixedWidth(QUITitleMinSize);
    widgetTitle->setFixedHeight(QUITitleMinSize);
    widgetTitle->setProperty("form", "title");
    widgetMain->setProperty("form", true);
    widgetMain->setProperty("canMove", true);

    //根据设定逐个追加属性
    widgetMain->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    if (tool) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::Tool);
    }
    if (top) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    if (menu) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    }

    IconHelper::Instance()->setIcon(labIco, QUIConfig::IconMain, QUIConfig::FontSize + 2);
    IconHelper::Instance()->setIcon(btnClose, QUIConfig::IconClose, QUIConfig::FontSize);
}

void QUIHelper::setSystemDateTime(const QString &year, const QString &month, const QString &day, const QString &hour, const QString &min, const QString &sec)
{
#ifdef Q_OS_WIN
    QProcess p(0);
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
#else
    QString cmd = QString("date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
    system(cmd.toLatin1());
    system("hwclock -w");
#endif
}

void QUIHelper::runWithSystem(const QString &strName, const QString &strPath, bool autoRun)
{
#ifdef Q_OS_WIN
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(strName, autoRun ? strPath : "");
#endif
}

QString QUIHelper::getIP(const QString &url)
{
    //取出IP地址
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    int start = regExp.indexIn(url);
    int length = regExp.matchedLength();
    QString ip = url.mid(start, length);
    return ip;
}

bool QUIHelper::isIP(const QString &ip)
{
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return regExp.exactMatch(ip);
}

bool QUIHelper::isMac(const QString &mac)
{
    QRegExp regExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
    return regExp.exactMatch(mac);
}

bool QUIHelper::isTel(const QString &tel)
{
    if (tel.length() != 11) {
        return false;
    }

    if (!tel.startsWith("13") && !tel.startsWith("14") && !tel.startsWith("15") && !tel.startsWith("18")) {
        return false;
    }

    return true;
}

bool QUIHelper::isEmail(const QString &email)
{
    if (!email.contains("@") || !email.contains(".com")) {
        return false;
    }

    return true;
}

QString QUIHelper::ipv4IntToString(quint32 ip)
{
    QString result = QString("%1.%2.%3.%4").arg((ip >> 24) & 0xFF).arg((ip >> 16) & 0xFF).arg((ip >> 8) & 0xFF).arg(ip & 0xFF);
    return result;
}

quint32 QUIHelper::ipv4StringToInt(const QString &ip)
{
    int result = 0;
    if (isIP(ip)) {
        QStringList list = ip.split(".");
        int ip0 = list.at(0).toInt();
        int ip1 = list.at(1).toInt();
        int ip2 = list.at(2).toInt();
        int ip3 = list.at(3).toInt();
        result = ip3 | ip2 << 8 | ip1 << 16 | ip0 << 24;
    }
    return result;
}

int QUIHelper::strHexToDecimal(const QString &strHex)
{
    bool ok;
    return strHex.toInt(&ok, 16);
}

int QUIHelper::strDecimalToDecimal(const QString &strDecimal)
{
    bool ok;
    return strDecimal.toInt(&ok, 10);
}

int QUIHelper::strBinToDecimal(const QString &strBin)
{
    bool ok;
    return strBin.toInt(&ok, 2);
}

QString QUIHelper::strHexToStrBin(const QString &strHex)
{
    uchar decimal = strHexToDecimal(strHex);
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len < 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrBin1(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();
    if (len <= 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrBin2(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();
    if (len <= 16) {
        for (int i = 0; i < 16 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrHex(int decimal)
{
    QString temp = QString::number(decimal, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }

    return temp;
}

QByteArray QUIHelper::intToByte(int i)
{
    QByteArray result;
    result.resize(4);
    result[3] = (uchar)(0x000000ff & i);
    result[2] = (uchar)((0x0000ff00 & i) >> 8);
    result[1] = (uchar)((0x00ff0000 & i) >> 16);
    result[0] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

QByteArray QUIHelper::intToByteRec(int i)
{
    QByteArray result;
    result.resize(4);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    result[2] = (uchar)((0x00ff0000 & i) >> 16);
    result[3] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

int QUIHelper::byteToInt(const QByteArray &data)
{
    int i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

int QUIHelper::byteToIntRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

quint32 QUIHelper::byteToUInt(const QByteArray &data)
{
    quint32 i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

quint32 QUIHelper::byteToUIntRec(const QByteArray &data)
{
    quint32 i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

QByteArray QUIHelper::ushortToByte(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[1] = (uchar)(0x000000ff & i);
    result[0] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

QByteArray QUIHelper::ushortToByteRec(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

int QUIHelper::byteToUShort(const QByteArray &data)
{
    int i = data.at(1) & 0x000000FF;
    i |= ((data.at(0) << 8) & 0x0000FF00);
    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

int QUIHelper::byteToUShortRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000FF;
    i |= ((data.at(1) << 8) & 0x0000FF00);
    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

QString QUIHelper::getXorEncryptDecrypt(const QString &value, char key)
{
    //矫正范围外的数据
    if (key < 0 || key >= 127) {
        key = 127;
    }

    QString result = value;
    int count = result.count();
    for (int i = 0; i < count; i++) {
        result[i] = QChar(result.at(i).toLatin1() ^ key);
    }
    return result;
}

uchar QUIHelper::getOrCode(const QByteArray &data)
{
    int len = data.length();
    uchar result = 0;
    for (int i = 0; i < len; i++) {
        result ^= data.at(i);
    }

    return result;
}

uchar QUIHelper::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    uchar temp = 0;
    for (uchar i = 0; i < len; i++) {
        temp += data.at(i);
    }

    return temp % 256;
}

//函数功能：计算CRC16
//参数1：*data 16位CRC校验数据，
//参数2：len   数据流长度
//参数3：init  初始化值
//参数4：table 16位CRC查找表

//正序CRC计算
quint16 QUIHelper::getCRC16(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 & 0xff;
        cRc_16 = (cRc_16 >> 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//逆序CRC计算
quint16 QUIHelper::getCRC16Rec(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 >> 8;
        cRc_16 = (cRc_16 << 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//Modbus CRC16校验
quint16 QUIHelper::getModbus16(quint8 *data, int len)
{
    //MODBUS CRC-16表 8005 逆序
    const quint16 table_16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    return getCRC16(data, len, 0xFFFF, table_16);
}

//CRC16校验
QByteArray QUIHelper::getCRCCode(const QByteArray &data)
{
    quint16 result = getModbus16((quint8 *)data.data(), data.length());
    return QUIHelper::ushortToByteRec(result);
}

static QMap<char, QString> listChar;
void QUIHelper::initAsciiStr()
{
    //0x20为空格,空格以下都是不可见字符
    if (listChar.count() == 0) {
        listChar.insert(0, "\\NUL");
        listChar.insert(1, "\\SOH");
        listChar.insert(2, "\\STX");
        listChar.insert(3, "\\ETX");
        listChar.insert(4, "\\EOT");
        listChar.insert(5, "\\ENQ");
        listChar.insert(6, "\\ACK");
        listChar.insert(7, "\\BEL");
        listChar.insert(8, "\\BS");
        listChar.insert(9, "\\HT");
        listChar.insert(10, "\\LF");
        listChar.insert(11, "\\VT");
        listChar.insert(12, "\\FF");
        listChar.insert(13, "\\CR");
        listChar.insert(14, "\\SO");
        listChar.insert(15, "\\SI");
        listChar.insert(16, "\\DLE");
        listChar.insert(17, "\\DC1");
        listChar.insert(18, "\\DC2");
        listChar.insert(19, "\\DC3");
        listChar.insert(20, "\\DC4");
        listChar.insert(21, "\\NAK");
        listChar.insert(22, "\\SYN");
        listChar.insert(23, "\\ETB");
        listChar.insert(24, "\\CAN");
        listChar.insert(25, "\\EM");
        listChar.insert(26, "\\SUB");
        listChar.insert(27, "\\ESC");
        listChar.insert(28, "\\FS");
        listChar.insert(29, "\\GS");
        listChar.insert(30, "\\RS");
        listChar.insert(31, "\\US");
        listChar.insert(0x5C, "\\");
        listChar.insert(0x7F, "\\DEL");
    }
}

QString QUIHelper::byteArrayToAsciiStr(const QByteArray &data)
{
    initAsciiStr();
    QString temp;
    int len = data.size();

    for (int i = 0; i < len; i++) {
        char byte = data.at(i);
        QString value = listChar.value(byte);
        if (!value.isEmpty()) {
        } else if (byte >= 0 && byte <= 0x7F) {
            value = QString("%1").arg(byte);
        } else {
            value = decimalToStrHex((quint8)byte);
            value = QString("\\x%1").arg(value.toUpper());
        }

        temp += value;
    }

    return temp.trimmed();
}

QByteArray QUIHelper::asciiStrToByteArray(const QString &data)
{
    initAsciiStr();
    QByteArray buffer;
    QStringList list = data.split("\\");

    int count = list.count();
    for (int i = 1; i < count; i++) {
        QString str = list.at(i);
        int key = 0;
        if (str.contains("x")) {
            key = strHexToDecimal(str.mid(1, 2));
        } else {
            key = listChar.key("\\" + str);
        }

        buffer.append(key);
    }

    return buffer;
}

char QUIHelper::hexStrToChar(char data)
{
    if ((data >= '0') && (data <= '9')) {
        return data - 0x30;
    } else if ((data >= 'A') && (data <= 'F')) {
        return data - 'A' + 10;
    } else if ((data >= 'a') && (data <= 'f')) {
        return data - 'a' + 10;
    } else {
        return (-1);
    }
}

QByteArray QUIHelper::hexStrToByteArray(const QString &data)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = data.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = data.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = data.at(i).toLatin1();
        hexdata = hexStrToChar(hstr);
        lowhexdata = hexStrToChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);
    return senddata;
}

QString QUIHelper::byteArrayToHexStr(const QByteArray &data)
{
    QString temp = "";
    QString hex = data.toHex();
    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    return temp.trimmed().toUpper();
}

QString QUIHelper::getSaveName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getSaveFileName(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::getFileName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileName(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::saveFileName(const QString &filter, const QString &defaultDir, const QString &fileName)
{
    QString file;
    QFileDialog dialog;
    dialog.setFixedSize(900, 600);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowTitle("保存文件");
    dialog.setLabelText(QFileDialog::Accept, "保存(&S)");
    dialog.setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog.selectFile(fileName);
    dialog.setNameFilter(filter);
    dialog.setDirectory(defaultDir);

    if (dialog.exec() == 1) {
        file = dialog.selectedFiles().value(0);
        file = QFileInfo(file).suffix().isEmpty() ? "" : file;
    }

    return file;
}

QStringList QUIHelper::getFileNames(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileNames(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::getFolderName()
{
    return QFileDialog::getExistingDirectory();
}

QString QUIHelper::getFileNameWithExtension(const QString &strFilePath)
{
    QFileInfo fileInfo(strFilePath);
    return fileInfo.fileName();
}

QStringList QUIHelper::getFolderFileNames(const QStringList &filter)
{
    QStringList fileList;
    QString strFolder = QFileDialog::getExistingDirectory();
    if (!strFolder.length() == 0) {
        QDir myFolder(strFolder);
        if (myFolder.exists()) {
            fileList = myFolder.entryList(filter);
        }
    }

    return fileList;
}

bool QUIHelper::folderIsExist(const QString &strFolder)
{
    QDir tempFolder(strFolder);
    return tempFolder.exists();
}

bool QUIHelper::fileIsExist(const QString &strFile)
{
    QFile tempFile(strFile);
    return tempFile.exists();
}

bool QUIHelper::copyFile(const QString &sourceFile, const QString &targetFile)
{
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

void QUIHelper::deleteDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}

bool QUIHelper::ipLive(const QString &ip, int port, int timeout)
{
    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    QObject::connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(ip, port);
    eventLoop.exec();
    bool ok = (tcpSocket.state() == QAbstractSocket::ConnectedState);
    return ok;
}

QString QUIHelper::getHtml(const QString &url)
{
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop eventLoop;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return QString(data);
}

QString QUIHelper::getNetIP(const QString &html)
{
    QString ip;
    QStringList list = html.split(" ");
    foreach (QString str, list) {
        //value=\"101.86.197.178\">
        if (str.contains("value=")) {
            QStringList temp = str.split("\"");
            ip = temp.at(1);
            break;
        }
    }
    return ip;
}

QString QUIHelper::getLocalIP()
{
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    QStringList ips = getLocalIPs();
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            break;
        }
    }

    return ip;
}

QStringList QUIHelper::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef emsdk
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
            if (flag) {
                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                foreach (QNetworkAddressEntry addr, addrs) {
                    //只取出IPV4的地址
                    if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        QString ip4 = addr.ip().toString();
                        if (ip4 != "127.0.0.1") {
                            ips << ip4;
                        }
                    }
                }
            }
        }
#endif
    }

    return ips;
}

QString QUIHelper::urlToIP(const QString &url)
{
    QHostInfo host = QHostInfo::fromName(url);
    return host.addresses().at(0).toString();
}

QString QUIHelper::getValue(quint8 value)
{
    QString result = QString::number(value);
    if (result.length() <= 1) {
        result = QString("0%1").arg(result);
    }
    return result;
}

bool QUIHelper::isWebOk()
{
    //能接通百度IP 115.239.211.112 说明可以通外网
    return ipLive("www.baidu.com", 80);
}

bool QUIHelper::isCustomUI = false;
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
    if (isCustomUI) {
        if (exec) {
            QUIMessageBox msg;
            msg.setMessage(info, 0, closeSec);
            msg.exec();
        } else {
            QUIMessageBox::Instance()->setMessage(info, 0, closeSec);
            QUIMessageBox::Instance()->show();
        }
    } else {
        QMessageBox box(QMessageBox::Information, "提示", info);
        box.setStandardButtons(QMessageBox::Yes);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.exec();
        //QMessageBox::information(0, "提示", info, QMessageBox::Yes);
    }
}

void QUIHelper::showMessageBoxError(const QString &info, int closeSec, bool exec)
{
    if (isCustomUI) {
        if (exec) {
            QUIMessageBox msg;
            msg.setMessage(info, 2, closeSec);
            msg.exec();
        } else {
            QUIMessageBox::Instance()->setMessage(info, 2, closeSec);
            QUIMessageBox::Instance()->show();
        }
    } else {
        QMessageBox box(QMessageBox::Critical, "错误", info);
        box.setStandardButtons(QMessageBox::Yes);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.exec();
        //QMessageBox::critical(0, "错误", info, QMessageBox::Yes);
    }
}

int QUIHelper::showMessageBoxQuestion(const QString &info)
{
    if (isCustomUI) {
        QUIMessageBox msg;
        msg.setMessage(info, 1);
        return msg.exec();
    } else {
        QMessageBox box(QMessageBox::Question, "询问", info);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.setButtonText(QMessageBox::No, QString("取 消"));
        return box.exec();
        //return QMessageBox::question(0, "询问", info, QMessageBox::Yes | QMessageBox::No);
    }
}

void QUIHelper::showTipBox(const QString &title, const QString &tip, bool fullScreen, bool center, int closeSec)
{
    QUITipBox::Instance()->setTip(title, tip, fullScreen, center, closeSec);
    QUITipBox::Instance()->show();
}

void QUIHelper::hideTipBox()
{
    QUITipBox::Instance()->hide();
}

QString QUIHelper::showInputBox(const QString &title, int type, int closeSec,
                                const QString &placeholderText, bool pwd,
                                const QString &defaultValue)
{
    if (isCustomUI) {
        QUIInputBox input;
        input.setParameter(title, type, closeSec, placeholderText, pwd, defaultValue);
        input.exec();
        return input.getValue();
    } else {
        return QInputDialog::getText(0, "输入框", title);
    }
}

void QUIHelper::showDateSelect(QString &dateStart, QString &dateEnd, const QString &format)
{
    QUIDateSelect select;
    select.setFormat(format);
    select.exec();
    dateStart = select.getStartDateTime();
    dateEnd = select.getEndDateTime();
}

void QUIHelper::initTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast)
{
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
    //行标题最大高度
    tableView->horizontalHeader()->setMaximumHeight(rowHeight);
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
    if (fileName.isEmpty()) {
        return;
    }
    if (QUIHelper::showMessageBoxQuestion(msg + "成功!确定现在就打开吗?") == QMessageBox::Yes) {
        QString url = QString("file:///%1").arg(fileName);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

bool QUIHelper::checkRowCount(int rowCount, int maxCount, int warnCount)
{
    if (rowCount > maxCount) {
        QString msg = QString("要处理的数据共 %1 条,超过最大 %2 条,请重新查询!").arg(rowCount).arg(maxCount);
        QUIHelper::showMessageBoxError(msg, 3);
        return false;
    }

    if (rowCount > warnCount) {
        QString msg = QString("要处理的数据共 %1 条,请耐心等待,确定要继续吗?").arg(rowCount);
        if (QUIHelper::showMessageBoxQuestion(msg) != QMessageBox::Yes) {
            return false;
        }
    }

    return true;
}
