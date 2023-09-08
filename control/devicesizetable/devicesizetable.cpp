#pragma execution_character_set("utf-8")

#include "devicesizetable.h"
#include "qprocess.h"
#include "qtablewidget.h"
#include "qheaderview.h"
#include "qfileinfo.h"
#include "qdir.h"
#include "qprogressbar.h"
#include "qtimer.h"
#include "qdebug.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif
#define GB (1024 * 1024 * 1024)
#define MB (1024 * 1024)
#define KB (1024)

DeviceSizeTable::DeviceSizeTable(QWidget *parent) : QTableWidget(parent)
{
    bgColor = QColor(255, 255, 255);

    chunkColor1 = QColor(100, 184, 255);
    chunkColor2 = QColor(24, 189, 155);
    chunkColor3 = QColor(255, 107, 107);

    textColor1 = QColor(10, 10, 10);
    textColor2 = QColor(255, 255, 255);
    textColor3 = QColor(255, 255, 255);

#if defined(Q_OS_UNIX) && !defined(Q_OS_WASM)
    process = new QProcess(this);
    connect(process, SIGNAL(readyRead()), this, SLOT(readData()));
#endif

    this->clear();

    //设置列数和列宽
    this->setColumnCount(5);
    this->setColumnWidth(0, 100);
    this->setColumnWidth(1, 120);
    this->setColumnWidth(2, 120);
    this->setColumnWidth(3, 120);
    this->setColumnWidth(4, 120);

    this->setStyleSheet("QTableWidget::item{padding:0px;}");

    QStringList headText;
    headText << "盘符" << "已用空间" << "可用空间" << "总大小" << "已用百分比" ;

    this->setHorizontalHeaderLabels(headText);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->verticalHeader()->setVisible(true);
    this->horizontalHeader()->setStretchLastSection(true);
    QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection);
    //QTimer::singleShot(10, this, SLOT(load()));
}

void DeviceSizeTable::readData()
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_WASM)
    while (!process->atEnd()) {
        QString result = QLatin1String(process->readLine());
#ifdef __arm__
        if (result.startsWith("/dev/root")) {
            checkSize(result, "本地存储");
        } else if (result.startsWith("/dev/mmcblk")) {
            checkSize(result, "本地存储");
        } else if (result.startsWith("/dev/mmcblk1p")) {
            checkSize(result, "SD卡");
            QStringList list = result.split(" ");
            emit sdcardReceive(list.at(0));
        } else if (result.startsWith("/dev/sd")) {
            checkSize(result, "U盘");
            QStringList list = result.split(" ");
            emit udiskReceive(list.at(0));
        }
#else
        if (result.startsWith("/dev/sd")) {
            checkSize(result, "");
            QStringList list = result.split(" ");
            emit udiskReceive(list.at(0));
        }
#endif
    }
#endif
}

void DeviceSizeTable::checkSize(const QString &result, const QString &name)
{
    QString dev, use, free, all;
    int percent = 0;
    QStringList list = result.split(" ");
    int index = 0;

    for (int i = 0; i < list.count(); ++i) {
        QString s = list.at(i).trimmed();
        if (s.isEmpty()) {
            continue;
        }

        index++;
        if (index == 1) {
            dev = s;
        } else if (index == 2) {
            all = s;
        } else if (index == 3) {
            use = s;
        } else if (index == 4) {
            free = s;
        } else if (index == 5) {
            percent = s.left(s.length() - 1).toInt();
            break;
        }
    }

    if (name.length() > 0) {
        dev = name;
    }

    insertSize(dev, use, free, all, percent);
}

void DeviceSizeTable::insertSize(const QString &name, const QString &use, const QString &free, const QString &all, int percent)
{
    int row = this->rowCount();
    this->insertRow(row);

    QTableWidgetItem *itemname = new QTableWidgetItem(name);
    QTableWidgetItem *itemuse = new QTableWidgetItem(use);
    itemuse->setTextAlignment(Qt::AlignCenter);
    QTableWidgetItem *itemfree = new QTableWidgetItem(free);
    itemfree->setTextAlignment(Qt::AlignCenter);
    QTableWidgetItem *itemall = new QTableWidgetItem(all);
    itemall->setTextAlignment(Qt::AlignCenter);

    this->setItem(row, 0, itemname);
    this->setItem(row, 1, itemuse);
    this->setItem(row, 2, itemfree);
    this->setItem(row, 3, itemall);

    QProgressBar *bar = new QProgressBar;
    bar->setRange(0, 100);
    bar->setValue(percent);

    QString qss = QString("QProgressBar{background:%1;border-width:0px;border-radius:0px;text-align:center;}"
                          "QProgressBar::chunk{border-radius:0px;}").arg(bgColor.name());

    if (percent < 50) {
        qss += QString("QProgressBar{color:%1;}QProgressBar::chunk{background:%2;}").arg(textColor1.name()).arg(chunkColor1.name());
    } else if (percent < 90) {
        qss += QString("QProgressBar{color:%1;}QProgressBar::chunk{background:%2;}").arg(textColor2.name()).arg(chunkColor2.name());
    } else {
        qss += QString("QProgressBar{color:%1;}QProgressBar::chunk{background:%2;}").arg(textColor3.name()).arg(chunkColor3.name());
    }

    bar->setStyleSheet(qss);
    this->setCellWidget(row, 4, bar);
}

QSize DeviceSizeTable::sizeHint() const
{
    return QSize(500, 300);
}

QSize DeviceSizeTable::minimumSizeHint() const
{
    return QSize(200, 150);
}

QColor DeviceSizeTable::getBgColor() const
{
    return this->bgColor;
}

void DeviceSizeTable::setBgColor(const QColor &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
        this->load();
    }
}

QColor DeviceSizeTable::getChunkColor1() const
{
    return this->chunkColor1;
}

void DeviceSizeTable::setChunkColor1(const QColor &chunkColor1)
{
    if (this->chunkColor1 != chunkColor1) {
        this->chunkColor1 = chunkColor1;
        this->load();
    }
}

QColor DeviceSizeTable::getChunkColor2() const
{
    return this->chunkColor2;
}

void DeviceSizeTable::setChunkColor2(const QColor &chunkColor2)
{
    if (this->chunkColor2 != chunkColor2) {
        this->chunkColor2 = chunkColor2;
        this->load();
    }
}

QColor DeviceSizeTable::getChunkColor3() const
{
    return this->chunkColor3;
}

void DeviceSizeTable::setChunkColor3(const QColor &chunkColor3)
{
    if (this->chunkColor3 != chunkColor3) {
        this->chunkColor3 = chunkColor3;
        this->load();
    }
}

QColor DeviceSizeTable::getTextColor1() const
{
    return this->textColor1;
}

void DeviceSizeTable::setTextColor1(const QColor &textColor1)
{
    if (this->textColor1 != textColor1) {
        this->textColor1 = textColor1;
        this->load();
    }
}

QColor DeviceSizeTable::getTextColor2() const
{
    return this->textColor2;
}

void DeviceSizeTable::setTextColor2(const QColor &textColor2)
{
    if (this->textColor2 != textColor2) {
        this->textColor2 = textColor2;
        this->load();
    }
}

QColor DeviceSizeTable::getTextColor3() const
{
    return this->textColor3;
}

void DeviceSizeTable::setTextColor3(const QColor &textColor3)
{
    if (this->textColor3 != textColor3) {
        this->textColor3 = textColor3;
        this->load();
    }
}

void DeviceSizeTable::load()
{
    //清空原有数据
    int row = this->rowCount();
    for (int i = 0; i < row; ++i) {
        this->removeRow(0);
    }

#ifdef Q_OS_WIN
    QFileInfoList list = QDir::drives();
    foreach (QFileInfo dir, list) {
        QString dirName = dir.absolutePath();
        LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
        ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;

        if (GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes)) {
            QString use = QString::number((double)(liTotalBytes.QuadPart - liTotalFreeBytes.QuadPart) / GB, 'f', 1);
            use += "G";
            QString free = QString::number((double) liTotalFreeBytes.QuadPart / GB, 'f', 1);
            free += "G";
            QString all = QString::number((double) liTotalBytes.QuadPart / GB, 'f', 1);
            all += "G";
            int percent = 100 - ((double)liTotalFreeBytes.QuadPart / liTotalBytes.QuadPart) * 100;
            insertSize(dirName, use, free, all, percent);
        }
    }
#elif defined(Q_OS_UNIX) && !defined(Q_OS_WASM)
    process->start("df", QStringList() << "-h");
#endif
}
