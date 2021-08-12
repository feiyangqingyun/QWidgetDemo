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
    uuid.replace("{", "");
    uuid.replace("}", "");
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

void QUIHelper::setStyle()
{
    //打印下所有内置风格的名字
    qDebug() << "Qt内置的样式" << QStyleFactory::keys();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qApp->setStyle(QStyleFactory::create("Fusion"));
#else
    qApp->setStyle(QStyleFactory::create("Cleanlooks"));
#endif
    //qApp->setPalette(QPalette("#FFFFFF"));
}

void QUIHelper::setFont(int fontSize)
{
    QFont font;
    font.setFamily("MicroSoft Yahei");
#ifdef Q_OS_ANDROID
    font.setPixelSize(15);
#elif __arm__
    font.setPixelSize(25);
#else
    font.setPixelSize(fontSize);
#endif

#ifndef rk3399
    qApp->setFont(font);
#endif
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

void QUIHelper::setTranslator(const QString &qmFile)
{
    QTranslator *translator = new QTranslator(qApp);
    translator->load(qmFile);
    qApp->installTranslator(translator);
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
    if (fileName.isEmpty()) {
        return;
    }
    if (QUIHelper::showMessageBoxQuestion(msg + "成功!确定现在就打开吗?") == QMessageBox::Yes) {
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
                if (list.at(1) == "") {
                    qDebug() << "ini node no value" << list.at(0);
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
